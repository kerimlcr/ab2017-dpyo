// VolumeScanner.cs
// 
// Copyright (C) 2008 - 2012 Patrick Ulbrich
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

using System;
using System.Text;
using System.Collections.Generic;
using IO = System.IO;
using Gtk;
using Gdk;
using Basenji.Gui.Widgets;
using Basenji.Gui.Widgets.Editors;
using Platform.Common.IO;
using VolumeDB;
using VolumeDB.VolumeScanner;
using VolumeDB.Metadata;

namespace Basenji.Gui
{
	public partial class VolumeScanner : Base.WindowBase
	{
		private const IconSize ICON_SIZE = IconSize.Menu;
		
		private Gdk.Pixbuf infoIcon;
		private Gdk.Pixbuf warningIcon;
		private Gdk.Pixbuf errorIcon;
		
		private ListStore				logStore;
		private VolumeDatabase			database;
		private IVolumeScanner			scanner;
		private MetadataProvider[]		mdps;
		private StringBuilder			scannerLog;
		private StatusUpdateTimer		timer;
		private volatile bool			scanCompleted;
		private Volume					newVolume;

		public VolumeScanner(VolumeDatabase db, DriveInfo drive) {
			this.scanCompleted = false;
			this.database = db;
			this.newVolume = null;
			
			infoIcon	= RenderIcon(Icons.Icon.Stock_DialogInfo,	   ICON_SIZE);
			warningIcon = RenderIcon(Icons.Icon.Stock_DialogWarning,   ICON_SIZE);
			errorIcon	= RenderIcon(Icons.Icon.Stock_DialogError,	   ICON_SIZE);
			
			mdps = null;
			
			if (App.Settings.ScannerExtractMetaData && 
			    (VolumeProber.ProbeVolume(drive) == VolumeProber.VolumeProbeResult.Filesystem)) {
				
				mdps = new MetadataProvider[] {				
					new TagLibMetadataProvider(),
					new ArchiveMetadataProvider()
				};
			}
			
			// setup scanner options
			ScannerOptions[] opts = new ScannerOptions[2] {
			
				new FilesystemScannerOptions() {
					BufferSize			= App.Settings.ScannerBufferSize,
					ComputeHashs		= App.Settings.ScannerComputeHashs,
					DiscardSymLinks		= App.Settings.ScannerDiscardSymLinks,
					GenerateThumbnails	= App.Settings.ScannerGenerateThumbnails,
					MetadataProviders	= mdps,
					DbDataPath			= PathUtil.GetDbDataPath(database)
				},
			
				new AudioCdScannerOptions() {
					EnableMusicBrainz = App.Settings.ScannerEnableMusicBrainz
				}
			};
			
			scanner = VolumeProber.GetScannerForVolume(drive, database, opts);
			
			// scanner eventhandlers
			scanner.BeforeScanItem	  += scanner_BeforeScanItem;
			scanner.ScannerWarning	  += scanner_ScannerWarning;
			scanner.Error			  += scanner_Error;
			scanner.ScanCompleted	  += scanner_ScanCompleted;

			/* volumedatabase event handlers */
			database.BeginWriteAccess	+= database_BeginWriteAccess;
			database.EndWriteAccess		+= database_EndWriteAccess;
			
			// must be called _after_ scanner instanciation 
			// (requires scanner.VolumeInfo.GetVolumeType())
			BuildGui();
			InitTreeView();
			
			scannerLog = new StringBuilder();
			timer = new StatusUpdateTimer(this);
			
			try {
				/* NOTE: make sure the timer will be removed properly later, 
				 * or it keeps running, even if this window has been closed. */
				timer.Install();
				
				string tmp;
				// e.g. GIO network 'drives' do not have a devicefile
				if (string.IsNullOrEmpty(drive.Device))
					tmp = S._("Scanning started.");
				else
					tmp = string.Format(S._("Scanning of drive '{0}' started."), drive.Device);
				
				UpdateLog(LogIcon.Info, tmp);
				
				switch (scanner.VolumeInfo.GetVolumeType()) {
					case VolumeType.FileSystemVolume:
						UpdateLog(LogIcon.Info, string.Format(S._("Options: generate thumbs: {0}, extract metadata: {1}, discard symlinks: {2}, hashing: {3}."),
						                                      BoolToStr(App.Settings.ScannerGenerateThumbnails),
					                                          BoolToStr(App.Settings.ScannerExtractMetaData),
					                                          BoolToStr(App.Settings.ScannerDiscardSymLinks),
					                                          BoolToStr(App.Settings.ScannerComputeHashs)));
						break;
					case VolumeType.AudioCdVolume:
						UpdateLog(LogIcon.Info, string.Format(S._("Options: MusicBrainz enabled: {0}"),
					                                      BoolToStr(App.Settings.ScannerEnableMusicBrainz)));
						break;
					default:
						throw new NotImplementedException(string.Format("Missing options output for scannertyp {0}", scanner.GetType()));
				}
				
				if (scanner.VolumeInfo.GetVolumeType() == VolumeType.FileSystemVolume) {
					// copy already known volume data into the editor
					volEditor.ArchiveNo = scanner.VolumeInfo.ArchiveNo;
					volEditor.Title = scanner.VolumeInfo.Title;
				} else {
					// other volumetypes have no useful data yet (e.g. AudioCD data may be queried from musicbrainz.org), 
					// so disable the editor and re-enable it and fill in the data when scanning has been completed.
					volEditor.Sensitive = false;
				}
				
				// start scanning on a new thread and return immediately
				scanner.RunAsync();
			} catch {
				timer.Remove();
				throw;
			}
		}
		
		private void InitTreeView() {
			TreeViewColumn col;

			col = new TreeViewColumn(string.Empty, new CellRendererPixbuf() { Xpad = 2 }, "pixbuf", 0);
			col.Expand = false;
			tvLog.AppendColumn(col);

			col = new TreeViewColumn(S._("Time"), new CellRendererText(), "text", 1);
			col.Expand = false;
			tvLog.AppendColumn(col);

			col = new TreeViewColumn(S._("Message"), new CellRendererText(), "text", 2);
			col.Expand = true;
			tvLog.AppendColumn(col);
			
			logStore = new ListStore(typeof(Gdk.Pixbuf), typeof(string), typeof(string));
			
			tvLog.Model = logStore;
		}
		
		public VolumeEditor VolumeEditor {
			get { return volEditor; }
		}
		
		private void UpdateLog(LogIcon icon, string message) {
			Pixbuf pb;
			string messageType;
			string timeStr = DateTime.Now.ToShortTimeString();

			switch(icon) {
				case LogIcon.Info:
					pb = infoIcon;
					messageType = "[ INFO  ]";
					break;
				case LogIcon.Warning:
					pb = warningIcon;
					messageType = "[WARNING]";
					break;
				case LogIcon.Error:
					pb = errorIcon;
					messageType = "[ ERROR ]";
					break;
				default:
					throw new Exception("Invalid LogIcon");
			}

			logStore.AppendValues(pb, timeStr, message);

			scannerLog.AppendFormat("[{0}] {1} {2}\n", timeStr, messageType, message);
		}		 
		
		private void SetStatus(string message) {
			statusbar.Pop(1);
			statusbar.Push(1, message);
		}
		
		private void SaveLog(long volumeID) {
			string dbDataPath = PathUtil.GetDbDataPath(database);
			string volumeDataPath = DbData.GetVolumeDataPath(dbDataPath, volumeID);
			
			if (!IO.Directory.Exists(volumeDataPath))
				IO.Directory.CreateDirectory(volumeDataPath);
			
			string logfile = IO.Path.Combine(volumeDataPath, "scanner.log");
			
			using (IO.StreamWriter w = new IO.StreamWriter(IO.File.OpenWrite(logfile))) {
				w.WriteLine(string.Format("{0} scanner log ({0} version: {1}, VolumeDB version: {2}) saved on {3}", 
										  App.Name, 
										  App.Version, 
										  Util.GetVolumeDBVersion(), 
										  DateTime.Now.ToString("yyyyy-MM-dd")));
				w.WriteLine();
				w.WriteLine(scannerLog.ToString());
			}
		}
		
		private bool SaveAndClose() {
			try {
				if (scanner.ScanSucceeded) {				
					// SaveTo() may throw a ValidationException.
					// Note: The volumeditor has either been filled on scanner start manually
					// (no volume loaded) or an existing volume has been loaded into it
					// (via the public VolumeEditor property or via volEditor.Load() on scan completion).
					volEditor.SaveTo(newVolume);
					SaveLog(newVolume.VolumeID);
					OnNewVolumeAdded(newVolume);
				}

				this.Destroy();
				
				if (scanner != null)
					scanner.Dispose();
				
				if (mdps != null) {
					foreach (var m in mdps)
						m.Dispose();
				}
			} catch (ValidationException e) {
				MsgDialog.ShowError(this, S._("Invalid data"), 
				                    string.Format(S._("\"{0}\" is {1}.\n\nExpected format: {2}\nPlease correct or remove the data you entered."), 
				                                  e.WidgetName, e.Message, e.ExpectedFormat));
				return false;			 
			}
			return true;
		}
		
		private void AbortScan() {
			if (!scanCompleted && btnAbort.Sensitive) {
				UpdateLog(LogIcon.Info, S._("Stopping Scanner and performing rollback..."));
				
				scanner.CancelAsync();
				/* disable button, 
				 * it will be enabled and converted to a closebutton when the ScanCompleted event is triggered */
				btnAbort.Sensitive = false;
			}
		}
		
		private static string BoolToStr(bool val) {
			return val ? S._("yes") : S._("no");
		}
		
		public event NewVolumeAddedEventHandler NewVolumeAdded;
		
		protected virtual void OnNewVolumeAdded(Volume volume) {
			if (NewVolumeAdded != null)
				NewVolumeAdded(this, new NewVolumeAddedEventArgs(volume));
		}
		
		#region window event handlers
		private void OnObjectDestroyed(object o, EventArgs args) {
			/* remove event handlers from the extern VolumeDatabase object */
			database.BeginWriteAccess	-= database_BeginWriteAccess;
			database.EndWriteAccess		-= database_EndWriteAccess;
			
			//// remove timeout handler (installed in ctor)
			//// TODO : comment from GUI_PUSH.txt
			//m_timer.Remove();
		}
		
		private void OnDeleteEvent(object o, Gtk.DeleteEventArgs args) {
			if (!scanCompleted) {		 
				MsgDialog.ShowError(this, S._("Scan in progress"), S._("You must stop scanning before closing this window."));
				args.RetVal = true;
			} else {
				bool cancel = !SaveAndClose(); 
				args.RetVal = cancel;
			}
		}
		
		[GLib.ConnectBefore()]
		private void OnWindowKeyPressEvent(object o, Gtk.KeyPressEventArgs args) {
			if (args.Event.Key == Gdk.Key.Escape) {
				if (!scanCompleted)
					AbortScan();
				else
					SaveAndClose();
			}
		}

		private void OnBtnAbortClicked(object sender, System.EventArgs args) {
			if (!scanCompleted)
				AbortScan();
			else
				SaveAndClose();
		}
		#endregion
		
		#region VolumeDatabase event handlers (executed on the scanner thread ?)
		// TODO : 
		// what is the executing thread of those VolumeDB events rised on the VolumeDB by the VolumeScanner?
		// in case of a different thread, 
		// is the VolumeScanner required to raise the event on the the tread of the VolumeDB?
		private void database_BeginWriteAccess(object sender, EventArgs e) {
			timer.RequestLedUpdate(true); // LED on
		}
		
		private void database_EndWriteAccess(object sender, EventArgs e) {
			timer.RequestLedUpdate(false); // LED off
		}
		#endregion
		
		#region Scanner event handlers (executed on the current threadcontext (i.e. gtk = no specific context -> new thread))
		private void scanner_BeforeScanItem(object sender, BeforeScanItemEventArgs e) {
			Application.Invoke(delegate {
				SetStatus(e.ItemName);
			});
		}

		private void scanner_ScannerWarning(object sender, ScannerWarningEventArgs e) {
			Application.Invoke(delegate {
				UpdateLog(LogIcon.Warning, e.Message);
			});
		}

		private void scanner_Error(object sender, ErrorEventArgs e) {
			Application.Invoke(delegate {
				UpdateLog(LogIcon.Error, string.Format(S._("An unhandled exception occured ({0})."), e.Exception.Message));
				UpdateLog(LogIcon.Info, S._("All database changes have been rolled back."));
			});
		}

		private void scanner_ScanCompleted(object sender, ScanCompletedEventArgs e) {
			Application.Invoke(delegate {
				if (e.Error != null) {
					UpdateLog(LogIcon.Error, string.Format(S._("Scanning failed. Reason: an unhandled exception occured ({0})."), e.Error.Message));
				} else if (e.Cancelled) {
					UpdateLog(LogIcon.Error, S._("Scanning aborted."));
				} else {
					UpdateLog(LogIcon.Info, S._("Scanning completed successfully."));
					newVolume = e.Volume;

					// the volume editor may have been disabled in the ctor 
					// for some volume types (e.g. AudioCD volumes)
					if (!volEditor.Sensitive) {
						volEditor.Load(newVolume);
						volEditor.Sensitive = true;
					}
				}

				if (!btnAbort.Sensitive) /* possibly disabled in AbortScan() */
					btnAbort.Sensitive = true;

				btnAbort.Label = Stock.Close;
				scanCompleted = true;
			});

			/* remove timeout handler (installed in ctor) */
			timer.Remove();
		}
		#endregion

		
		/* 
		 * Timer class that pulls scanner status values at a given interval
		 * and updates the gui with them.
		 */
		private class StatusUpdateTimer
		{
			/* Assuming a display refresh rate (frequency) of at least 70 Hz,
			 * the GUI update resolution shouldn't be smaller than (1000ms / 70Hz) = ~14ms */
			private const uint TIMEOUT_INTERVAL = 20;

			private VolumeScanner	vscanner;
			private volatile bool	remove;
			private volatile bool	ledOnRequest;
			private volatile bool	ledOffRequest;
			private long			lastLedUpdate;
			
			public StatusUpdateTimer(VolumeScanner vs) {
				this.vscanner	= vs;
				this.remove		= true;
			}

			public void Install() {
				if (!remove)
					return;

				remove					= false;
				vscanner.led.LedState	= false;
				ledOnRequest			= false;
				ledOffRequest			= false;

				GLib.Timeout.Add(TIMEOUT_INTERVAL, delegate {
					/* get local copies of thread shared vars */
					bool persist = !remove;
					bool ledOnReq = ledOnRequest;
					bool ledOffReq = ledOffRequest;
					
					/* 
					 * update counter labels (expected to take less than 1ms)
					 */
					if (vscanner.scanner != null)
						vscanner.volEditor.UpdateInfo(vscanner.scanner.VolumeInfo);

					/* 
					 * update database write access indicator LED
					 */
					if (!persist) {
						/* switch off the LED when the timer is going to be removed */
						vscanner.led.LedState = false;
					} else {
						if (ledOnReq) {
							if (!vscanner.led.LedState)
								vscanner.led.LedState = true; /* toggle LED state */
							
							ledOnRequest = false;
							lastLedUpdate = DateTime.Now.Ticks;
						}
						
						/* switch off the LED if it hasn't been switched on during the last ~300 ms.
						 * (i.e. make the LED glow for at least 300 ms) */
						if ( vscanner.led.LedState && ledOffReq &&
							(((DateTime.Now.Ticks - lastLedUpdate) / 10000) >= 300) )
							vscanner.led.LedState = false;
					}
					
					return persist;
				});
			}

			public void Remove() {
				remove = true;
			}

			public void RequestLedUpdate(bool on) {
				if (on) {
					/* remove possibly pending previous switch-off request */
					ledOffRequest = false;
					
					/* request LED switch-on */
					ledOnRequest = true;
					
				} else {
					/* don't disable the ledOnRequest here !*/
					
					/* request LED switch-off */
					ledOffRequest = true;
				}
			}
		}

		private enum LogIcon {
			Info	= 0,
			Warning = 1,
			Error	= 2
		}

	}
	
	// gui initialization
	public partial class VolumeScanner : Base.WindowBase
	{
		private TreeView				tvLog;
		private Button					btnAbort;
		private Statusbar				statusbar;
		private Led						led;
		private VolumeEditor			volEditor;
		
		protected override void BuildGui () {
			base.BuildGui();
			
			// general window settings
			SetModal();
			//this.DefaultWidth		= 580;
			this.DefaultHeight		= 600;
			this.Title				= S._("VolumeScanner");
			
			// vbOuter
			VBox vbOuter = new VBox();
			vbOuter.Spacing = 0;
			
			// vbVolEditor
			VBox vbVolEditor = new VBox();
			vbVolEditor.Spacing = 6;
			vbVolEditor.BorderWidth = 12;
			
			volEditor = VolumeEditor.CreateInstance(scanner.VolumeInfo.GetVolumeType());
			vbVolEditor.PackStart(CreateLabel(S._("<b>Volume Information:</b>"), true, 0, 0), false, false, 0);
			vbVolEditor.PackStart(LeftAlign(volEditor), true, true, 0);
			
			vbOuter.PackStart(vbVolEditor, false, false, 0);
			
			// vbScannerLog
			VBox vbScannerLog = new VBox();
			vbScannerLog.Spacing = 6;
			vbScannerLog.BorderWidth = 12;
			
			vbScannerLog.PackStart(CreateLabel(S._("<b>Scanner Log:</b>"), true, 0, 0), false, false, 0);
			
			ScrolledWindow sw = CreateScrolledView<TreeView>(out tvLog, false);
			// set min width of the scrolled window widget
			sw.WidthRequest = 520;
			
			vbScannerLog.PackStart(LeftAlign(sw), true, true, 0);
			vbOuter.PackStart(vbScannerLog, true, true, 0);
			
			// hbbox
			HButtonBox hbbox = new HButtonBox();
			hbbox.Spacing = 6;
			hbbox.BorderWidth = 12;
			hbbox.LayoutStyle = ButtonBoxStyle.End;
			
			led = new Led(false);
			Alignment a = new Alignment(1.0f, 0.5f, 0.0f, 0.0f);
			led.TooltipText = S._("Database access");
			a.Add(led);
			hbbox.PackStart(a, false, false, 0);
			
			btnAbort = CreateButton(Stock.Cancel, true, OnBtnAbortClicked);
			hbbox.PackEnd(btnAbort, false, false, 0);

			vbOuter.PackStart(hbbox, false, false, 0);
			
			// statusbar
			statusbar = new Statusbar();
			statusbar.Spacing = 6;
			
			vbOuter.PackStart(statusbar, false, false, 0);
			
			this.Add(vbOuter);
			
			// event handlers
			this.KeyPressEvent += OnWindowKeyPressEvent;
			this.DeleteEvent	+= OnDeleteEvent;
			this.Destroyed		+= OnObjectDestroyed;
			
			ShowAll();
		}

	}
}
