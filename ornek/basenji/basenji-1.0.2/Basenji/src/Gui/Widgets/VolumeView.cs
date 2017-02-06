// VolumeView.cs
// 
// Copyright (C) 2008 - 2010 Patrick Ulbrich
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
using Gtk;
using Basenji.Gui.Base;
using Basenji.Gui.Widgets.Editors;
using Basenji.Icons;
using VolumeDB;

namespace Basenji.Gui.Widgets
{
	public enum VolumeSortProperty
	{
		VolumeID = 1,
		ArchiveNo = 2,
		Added = 3,
		Title = 4,
		DriveType = 5,
		Category = 6,
		/* Description = 7,
		Keywords = 8 */
	}
	
	public class VolumeView : ViewBase
	{
		private static readonly string STR_UNNAMED	= S._("Unnamed");
		private static readonly string STR_CATEGORY	= S._("Category:");
		private static readonly string STR_FILES	= S._("files");
		private static readonly string STR_TRACKS	= S._("tracks");
		
		private const IconSize ICON_SIZE = IconSize.Dialog;
		private IconCache iconCache;
		
		private VolumeSortProperty sortProperty;
		private bool toggleColumn;
		
		public VolumeView() {
			iconCache = new IconCache(this);
			sortProperty = VolumeSortProperty.VolumeID;
			toggleColumn = false;
			
			//
			// init columns
			//
			TreeViewColumn col;
			
			col = new TreeViewColumn(string.Empty, new CellRendererPixbuf(), "pixbuf", 0);			
			col.Expand = false;
			AppendColumn(col);
 
			col = new TreeViewColumn(string.Empty, new CellRendererText(), "markup", 1);
			col.Expand = true;
			AppendColumn(col);
			
			//
			// setup store
			//
			ListStore store = new Gtk.ListStore(typeof(Gdk.Pixbuf),
			                                    typeof(string),
			                                    /* Volume - not visible */
			                                    typeof(Volume));
			
			// must be assignet before 
			// assinging the sort func
			this.Model = store;			
			
			Gtk.TreeIterCompareFunc sortfunc = delegate(TreeModel m, TreeIter a, TreeIter b) {
				Volume vol_a = GetVolume(a);
				Volume vol_b = GetVolume(b);
			
				if (vol_a == null || vol_b == null)
					return 0;
				
				switch (sortProperty) {
					case VolumeSortProperty.Added:
						return Math.Sign(vol_a.Added.Subtract(vol_b.Added).Ticks);
					case VolumeSortProperty.ArchiveNo:
						return Sorting.NatCompare(vol_a.ArchiveNo, vol_b.ArchiveNo);
					case VolumeSortProperty.Category:
						return string.Compare(vol_a.Category, vol_b.Category);
					case VolumeSortProperty.DriveType:
						return Math.Sign(vol_a.DriveType - vol_b.DriveType);
					case VolumeSortProperty.Title:
						return string.Compare(vol_a.Title, vol_b.Title);
					case VolumeSortProperty.VolumeID:
						return Math.Sign(vol_a.VolumeID - vol_b.VolumeID);
					default: 
						throw new ArgumentException("Invalid VolumeSortProperty");
				}
				
				return 0;
			};
			
			store.SetSortFunc(0, sortfunc);
			store.SetSortFunc(1, sortfunc);
			
			// set initial sorting
			/* Sort(sortProperty, true); */
		}

		public void Fill(Volume[] volumes) {
			if (volumes == null)
				throw new ArgumentNullException("volumes");

			Clear();
			
			ListStore store = (ListStore)Model;
			
			foreach (Volume v in volumes)
				AddVolume(store, v);

			// cluumns may still have the old width after clear()
			// so reset it
			ColumnsAutosize();
		}
		
		public void SetSortProperty(VolumeSortProperty sortProperty, bool descending) {
			this.sortProperty = sortProperty;
			toggleColumn = !toggleColumn;
			((ListStore)Model).SetSortColumnId(toggleColumn ? 0 : 1, descending ? SortType.Descending : SortType.Ascending);
		}
		
		public void Clear() {
			if (Model != null) {
				ListStore store = (ListStore)Model;
				store.Clear();
			}
		}
		
		public Volume GetVolume(TreeIter iter) {
			Volume v = (Model.GetValue(iter, 2) as Volume);
			return v;
		}
		
		public void RemoveVolume(TreeIter iter) {
			Remove(iter);  
		}
		
		public void UpdateVolume(TreeIter iter, Volume volume) {
			//Model.SetValue(iter, 0, GetVolumeIcon(volume);
			Model.SetValue(iter, 1, GetVolumeDescription(volume));
		}
		
		public void AddVolume(Volume v) {
			AddVolume((ListStore)this.Model, v);
		}
				
		private void AddVolume(ListStore store, Volume v) {
			store.AppendValues(GetVolumeIcon(v), GetVolumeDescription(v), v);
		}
		
		private string GetVolumeDescription(Volume v) {
			// common volume info
			string title;
			string category;
			
		 	if (string.IsNullOrEmpty(v.Title)) {
				Gdk.Color a = Parent.Style.Base(Gtk.StateType.Normal);
				Gdk.Color b = Parent.Style.Text(Gtk.StateType.Normal);
				Gdk.Color c = Util.ColorBlend(a, b);
			
				double gdk_max = (double)ushort.MaxValue;
				string col = string.Format("#{0:X2}{1:X2}{2:X2}",
			                           (int)(255 * (c.Red / gdk_max)),
			                           (int)(255 * (c.Green / gdk_max)),
			                           (int)(255 * (c.Blue / gdk_max)));
				
				title = string.Format("<span foreground=\"{0}\">{1}</span>", col, Util.Escape(STR_UNNAMED));
			
			} else if (!string.IsNullOrEmpty(v.LoanedTo)) {
				title = string.Format("<span foreground=\"red\">{0}</span>", Util.Escape(v.Title));
			} else {
				title = Util.Escape(v.Title);
			}
		
			if (string.IsNullOrEmpty(v.ArchiveNo)) {
				title = string.Format("<b>{0}</b>", title);
			} else {
				title = string.Format("<b>{0}</b> <small>({1})</small>",
			                      title,
			                      Util.Escape(v.ArchiveNo));
			}
		
			if (string.IsNullOrEmpty(v.Category))
				category = "-";
			else if (!VolumeEditor.categories.TryGetTranslatedString(v.Category, out category))
				category = v.Category;
			
			// specific volume info			
			// only show important info, otherwise its too cluttered, too high!
			string strFormat = "{0}\n<span size=\"medium\"><i>{1}</i> {2}\n{3} / {4} {5}</span>";
			string desc;
			
			switch (v.GetVolumeType()) {
				case VolumeType.FileSystemVolume:
					FileSystemVolume fsv = (FileSystemVolume)v;
					
					desc = string.Format(strFormat,
				                     title,
				                     Util.Escape(STR_CATEGORY),
									 Util.Escape(category),
									 Util.GetSizeStr(fsv.Size),
				                     fsv.Files.ToString(),
				                     Util.Escape(STR_FILES));
					break;					
			case VolumeType.AudioCdVolume:
					AudioCdVolume avol = (AudioCdVolume)v;
				
					desc = string.Format(strFormat,
				                     title,
				                     Util.Escape(STR_CATEGORY),
									 Util.Escape(category),
									 avol.Duration,
				                     avol.Tracks.ToString(),
				                     Util.Escape(STR_TRACKS));
					break;
				default:
					throw new NotImplementedException("Description not implemented for this VolumeType");
			}
			
			return desc;
		}
		
		private Gdk.Pixbuf GetVolumeIcon(Volume v) {
			Gdk.Pixbuf icon;
			
			switch (v.DriveType) {
				case VolumeDriveType.CDRom:
					icon = iconCache.GetIcon(Icons.Icon.Stock_Cdrom, ICON_SIZE);
					break;
				case VolumeDriveType.Harddisk:
					icon = iconCache.GetIcon(Icons.Icon.Stock_Harddisk, ICON_SIZE);
					break;
				case VolumeDriveType.Ram:
					icon = iconCache.GetIcon(Icons.Icon.Stock_Harddisk, ICON_SIZE); // FIXME : is there a more suitable icon?
					break;
				case VolumeDriveType.Network:
					icon = iconCache.GetIcon(Icons.Icon.Stock_Network, ICON_SIZE);
					break;
				case VolumeDriveType.Removable:
					icon = iconCache.GetIcon(Icons.Icon.DriveRemovableMedia, ICON_SIZE);
					break;
				case VolumeDriveType.Unknown:
					icon = iconCache.GetIcon(Icons.Icon.Stock_Harddisk, ICON_SIZE); // FIXME : is there a more suitable icon?
					break;
			   default:
				   throw new Exception("Invalid VolumeDriveType");
			}
			
			return icon;
		}
	}
}