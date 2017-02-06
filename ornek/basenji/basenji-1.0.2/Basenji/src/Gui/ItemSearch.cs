// ItemSearch.cs
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
using Gtk;
using Basenji.Gui.Base;
using Basenji.Gui.Widgets;
using VolumeDB;
using VolumeDB.Searching;
using VolumeDB.Searching.ItemSearchCriteria;

namespace Basenji.Gui
{
	public partial class ItemSearch : WindowBase
	{	
		private MainWindow mainWindow;
		private VolumeDatabase database;
		private volatile bool windowDeleted;
		
		public ItemSearch(MainWindow mainWindow, VolumeDatabase db) {
			windowDeleted = false;			  
			this.mainWindow = mainWindow;
			this.database = db;
			BuildGui();
			btnSearch.Sensitive = false;
			txtSearchString.GrabFocus();
			
			// the widget should be visible the first time
			// when the user clicks on an item
			itemInfo.Hide();
		}
		
		private void SetStatus(string message) {
			statusbar.Pop(1);
			statusbar.Push(1, message);
		}
		
		private void BeginSearch() {
			// make sure searching is enabled 
			// (searchstring may be too short)			
			if (!btnSearch.Sensitive)
				return;
			
			ISearchCriteria criteria = null;
			try {
				criteria = new EUSLSearchCriteria(txtSearchString.Text);
			} catch (ArgumentException e) {				
				SetStatus(Util.FormatExceptionMsg(e));
				return;				
			}
			
			// callback called when searching has been finished
			AsyncCallback callback = ar => {
				if (windowDeleted)
					return;
				
				try {
					VolumeItem[] items = database.EndSearchItem(ar);
					
					Application.Invoke((o, args) => {						
						// calls selection changed handler
						// that fills the searchrestult view
						tvCategory.Categorize(items);

						TimeSpan time = DateTime.Now.Subtract((DateTime)ar.AsyncState);
						SetStatus(string.Format(S._("Found {0} items in {1:F3} seconds."), items.Length, time.TotalSeconds));
					});
				} catch (Exception e) {
					Application.Invoke((o, args) => {
						tvCategory.Clear();
						navi.Clear();
						tvSearchResult.Clear();
					});
					
					if (e is TimeoutException) {
						// couldn't get connection lock
						Application.Invoke((o, args) => SetStatus(S._("Timeout: another search is probably still in progress.")));
					} else if (e is TooManyResultsException) {						
						Application.Invoke((o, args) => SetStatus(S._("Too many search results. Please refine your search criteria.")));
					} else {
						//Application.Invoke((o, args) => SetStatus(Util.FormatExceptionMsg(e)));
						Application.Invoke((o, args) => SetStatus(string.Empty));
						throw;
					}
				} finally {
					Application.Invoke((o, args) => {
						itemInfo.Clear();
						itemInfo.Hide();
					});
				}
			};
			
			try {
				infoBar.Visible = false;
				navi.Visible = true;
				
				SetStatus(S._("Searching..."));
				database.BeginSearchItem(criteria, callback, DateTime.Now);
			} catch(Exception) {
				SetStatus(string.Empty);
				throw;			  
			}
		}
		
		private void EditItem() {
			TreeIter iter = TreeIter.Zero;
			
			if (!tvSearchResult.GetSelectedIter(out iter))
				return;
			
			// load item properties
			VolumeItem item = tvSearchResult.GetItem(iter);
			
			if (item == null)
				return;
			
			new ItemProperties(item);
		}
		
		private void ShowItemInMainWindow() {
			TreeIter iter = TreeIter.Zero;
			
			if (!tvSearchResult.GetSelectedIter(out iter))
				return;
			
			VolumeItem item = tvSearchResult.GetItem(iter);
			
			if (item == null)
				return;
			
			Destroy();
			
			mainWindow.FindItem(item);
		}
		
		private void Close() {
			App.Settings.ItemInfoMinimized2 = itemInfo.Minimized;
			windowDeleted = true;
			this.Destroy();
		}
		
		private void OnBtnSearchClicked(object o, System.EventArgs args) {
			BeginSearch();
		}
		
		private void OnTxtSearchStringSearch(object o, Widgets.SearchEventArgs args) {
			BeginSearch();
		}
		
		private void OnTxtSearchStringChanged(object o, EventArgs args) {
			btnSearch.Sensitive = ( (txtSearchString.Text.Length >= VolumeDatabase.MIN_SEARCHSTR_LENGTH) 
			                       && (txtSearchString.Text != txtSearchString.PlaceholderText) );
		}

		/*
		private void OnBtnCloseClicked(object sender, System.EventArgs e) {
			this.Destroy(); // TODO : not neccessary?		 
		}*/
		
		private void OnTvCategorySelectionChanged(object o, EventArgs args) {
			TreeIter iter;
			CategoryView.Category c = CategoryView.Category.None;
			if (tvCategory.GetSelectedIter(out iter))
				c = tvCategory.GetCategory(iter);
			
			VolumeItem[] categoryItems = tvCategory.GetCategoryItems(c);
			
			navi.SetItems(categoryItems);
			tvSearchResult.Fill(navi.PageItems, database, false);
			
			itemInfo.Clear();
			itemInfo.Hide();
		}
		
		private void OnNaviNavigate(object o, NavigateEventArgs args) {
			tvSearchResult.Fill(navi.PageItems, database, false);
			
			itemInfo.Clear();
			itemInfo.Hide();
		}
		
		[GLib.ConnectBefore()]
		private void OnTvSearchResultButtonPressEvent(object o, ButtonPressEventArgs args) {
			TreePath path;
			tvSearchResult.GetPathAtPos((int)args.Event.X, (int)args.Event.Y, out path);
			if (path == null)
				return;
				
			if ((args.Event.Button == 1) && (args.Event.Type == Gdk.EventType.TwoButtonPress)) {
				EditItem();
			} else if ((args.Event.Button == 3) && (args.Event.Type == Gdk.EventType.ButtonPress)) {
				uint btn = args.Event.Button;
				uint time = args.Event.Time;
				itemContextMenu.Popup(null, null, null, btn, time);
			}
		}
		
		[GLib.ConnectBefore()]
		private void OnTvSearchResultKeyPressEvent(object o, Gtk.KeyPressEventArgs args) {
			if (args.Event.Key == Gdk.Key.Return)
				EditItem();
		}
		
		private void OnTvSearchResultSelectionChanged(object o, EventArgs args) {
			TreeIter iter;
			if (!tvSearchResult.GetSelectedIter(out iter))
				return;
			
			VolumeItem item = tvSearchResult.GetItem(iter);
			if (item == null)
				return;
			
			if (App.Settings.ShowItemInfo) 
				itemInfo.ShowInfo(item, database);
			else if (itemInfo.Visible)
				itemInfo.Hide();
		}
		
		[GLib.ConnectBefore()]
		private void OnWindowKeyPressEvent(object o, Gtk.KeyPressEventArgs args) {
			if (args.Event.Key == Gdk.Key.Escape)
				Close();
		}
		
		private void OnDeleteEvent(object sender, DeleteEventArgs args) {
			Close();
		}
	}
	
	// gui initialization
	public partial class ItemSearch : WindowBase {
		
		private SearchEntry					txtSearchString;
		private Button						btnSearch;
		private CategoryView				tvCategory;
		private PageNavigation<VolumeItem>	navi;
		private InfoBar						infoBar;
		private SearchResultView			tvSearchResult;		
		private ItemInfo					itemInfo;
		private Statusbar					statusbar;
		private Menu						itemContextMenu;
		
		protected override void BuildGui() {
			base.BuildGui();

			bool itemInfoMinimized = App.Settings.ItemInfoMinimized2;
			
			// general window settings
			SetModal();
			this.DefaultWidth		= 800;
			this.DefaultHeight		= 600;
			this.Title				= S._("Search Items");
			this.Icon				= this.RenderIcon(Basenji.Icons.Icon.Stock_Find, IconSize.Menu);
			
			// vbOuter			  
			VBox vbOuter = new VBox();
			vbOuter.Spacing = 0;
			
			// search box
			HBox hbSearch = new HBox();
			hbSearch.Spacing = 6;
			hbSearch.BorderWidth = 6;
			
			string STR_DEFAULT = S._("default");
			
			txtSearchString = new SearchEntry();
			txtSearchString.ShowClearIcon = false;
			txtSearchString.PlaceholderText = S._("Search items");
			
			txtSearchString.SetPresets(new Widgets.SearchEntryPreset[] {
				new Widgets.SearchEntryPreset(string.Format("filename ({0})", STR_DEFAULT), "filename:", null),
				new Widgets.SearchEntryPreset(string.Format("directoryname ({0})", STR_DEFAULT), "directoryname:", null),
				new Widgets.SearchEntryPreset("location", "location:", null),
				new Widgets.SearchEntryPreset("note", "note:", null),
				new Widgets.SearchEntryPreset("keywords", "keywords:", null),
				new Widgets.SearchEntryPreset("metadata", "metadata:", null),
				new Widgets.SearchEntryPreset("volume-title", "volume-title:", null),
				new Widgets.SearchEntryPreset("type [audio, video, image, text, directory]", "type =", "audio"),
				new Widgets.SearchEntryPreset("filesize", "filesize", "> 1mb")
			});
			
			hbSearch.PackStart(txtSearchString.GetFallbackWrapper(), true, true, 0);
			
			btnSearch = CreateButton(Stock.Find, true, OnBtnSearchClicked);
			hbSearch.PackStart(btnSearch, false, false, 0); 
			
			vbOuter.PackStart(hbSearch, false, false, 0);
			
			// hpaned
			HPaned hpaned = new HPaned();
			hpaned.BorderWidth = 6;
			hpaned.Position = 200;
			
			// category
			ScrolledWindow swCategory = CreateScrolledView<CategoryView>(out tvCategory, true);
			hpaned.Pack1(swCategory, false, false);
			
			// search results
			VBox vbRight = new VBox();
			vbRight.Spacing = 6;
			
			infoBar = new InfoBar();
			infoBar.Headline = S._("Quick tip:");
			infoBar.Text = string.Format(S._("Click on the icon to the left of the search field to build advanced search queries.\nExample: {0}"), 
			                             @"<i>""holidays and type = video and filesize > 600mb""</i>");
			
			vbRight.PackStart(infoBar, false, false, 0);
			
			navi = new PageNavigation<VolumeItem>();
			navi.PageSize = App.Settings.SearchResultPageSize;
			navi.EmptyCaption = S._("No search results.");
			
			vbRight.PackStart(navi, false, false, 0);
			
			ScrolledWindow swSearchResult = CreateScrolledView<SearchResultView>(out tvSearchResult, true);
			vbRight.PackStart(swSearchResult, true, true, 0);
			
			// item info
			itemInfo = new ItemInfo();
			
			vbRight.PackStart(itemInfo, false, false, 0);
			
			hpaned.Pack2(vbRight, true, true);
			
			vbOuter.PackStart(hpaned, true, true, 0);
			
			// statusbar
			statusbar = new Statusbar();
			statusbar.Spacing = 6;
			vbOuter.PackStart(statusbar, false, false, 0);
			
			this.Add(vbOuter);
			
			// context menus
			itemContextMenu = CreateContextMenu();
			
			// event handlers
			txtSearchString.Search				+= OnTxtSearchStringSearch;
			txtSearchString.Changed				+= OnTxtSearchStringChanged;
			tvCategory.Selection.Changed		+= OnTvCategorySelectionChanged;
			navi.Navigate						+= OnNaviNavigate;
			tvSearchResult.ButtonPressEvent		+= OnTvSearchResultButtonPressEvent;
			tvSearchResult.KeyPressEvent		+= OnTvSearchResultKeyPressEvent;
			tvSearchResult.Selection.Changed	+= OnTvSearchResultSelectionChanged;
			
			this.KeyPressEvent					+= OnWindowKeyPressEvent;
			this.DeleteEvent					+= OnDeleteEvent;
				
			ShowAll();
			
			// must be called _after_ ShowAll()
			itemInfo.Minimized = itemInfoMinimized;
			navi.Visible = false;
		}
		
		private Menu CreateContextMenu() {
			ActionGroup ag = new ActionGroup("default");
			
			ag.Add(WindowBase.CreateAction("edititem",
			                               S._("Edit Item"),
			                               null,
			                               Stock.Edit,
			                               (o, args) => EditItem()),
			                               null);
			
			ag.Add(WindowBase.CreateAction("show_in_mainwindow",
			                               S._("Show in Main Window"),
			                               null,
			                               Stock.Directory,
			                               (o, args) => ShowItemInMainWindow()),
			                               null);
			
			UIManager manager = new UIManager();
			manager.InsertActionGroup(ag, 0);
			//this.AddAccelGroup(manager.AccelGroup);
			
			string ui = @"
				<popup name=""item_contextmenu"">
					<menuitem action=""edititem""/>
					<menuitem action=""show_in_mainwindow""/>
				</popup>";
			
			manager.AddUiFromString(ui);
			
			return (Menu)manager.GetWidget("/item_contextmenu");
		}
	}
}
