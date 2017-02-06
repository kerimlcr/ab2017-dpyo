// SearchResultView.cs
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
using System.Collections.Generic;
using Gtk;
using Basenji.Gui.Base;
using Basenji.Icons;
using VolumeDB;

namespace Basenji.Gui.Widgets
{
	public class SearchResultView : ViewBase
	{	
		private static readonly string STR_UNNAMED = S._("Unnamed");
		private static readonly string STR_LOCATION = S._("Location");
		private static readonly string STR_VOLUME = S._("Volume");
		private static readonly string STR_ARCHIVENO = S._("Archive No.");
		private static readonly string STR_AUDIOTRACK = S._("Audio CD track");
		
		private const IconSize ICON_SIZE = IconSize.Dialog;
		
		private ItemIcons itemIcons;
		private Dictionary<long, Volume> volumeCache;
		
		public SearchResultView() {
			itemIcons = new ItemIcons(this);
			volumeCache = new Dictionary<long,Volume>();
			
			//
			// setup columns
			//
			TreeViewColumn col;
			
			col = new TreeViewColumn(string.Empty, new CellRendererPixbuf(), "pixbuf", 0);
			col.Expand = false;
			AppendColumn(col);
 
			col = new TreeViewColumn(string.Empty, new CellRendererText(), "markup", 1);
			col.Expand = true;
			AppendColumn(col);
		}
		
		public void Fill(VolumeItem[] items, VolumeDatabase db, bool clearVolumeCache) {
			if (clearVolumeCache)
				volumeCache.Clear();
			Fill(items, db);
		}
		
		public void Fill(VolumeItem[] items, VolumeDatabase db) {
			if (items == null)
				throw new ArgumentNullException("items");
			
			if (db == null)
				throw new ArgumentNullException("db");
			
			ListStore store = new Gtk.ListStore(	typeof(Gdk.Pixbuf),
													typeof(string),
													typeof(VolumeItem)); /* VolumeItem - not visible */
			
			foreach (VolumeItem item in items) {
				Volume vol;
						
				if (!volumeCache.TryGetValue(item.VolumeID, out vol)) {
					vol = item.GetOwnerVolume();
					volumeCache.Add(vol.VolumeID, vol);						   
				}
				
				string description;
				string itemName = Util.Escape(item.Name);
				string volTitle = Util.Escape(vol.Title.Length > 0 ? vol.Title : STR_UNNAMED);
				string archiveNo = Util.Escape(vol.ArchiveNo.Length > 0 ? vol.ArchiveNo : "-");
				
				switch (item.GetVolumeItemType()) {
					case VolumeItemType.FileVolumeItem:					   
					case VolumeItemType.DirectoryVolumeItem:
						description = string.Format("<b>{0}</b>\n<span size=\"smaller\"><i>{1}:</i> {2}\n<i>{3}:</i> {4}, <i>{5}:</i> {6}</span>",
					                            itemName,
					                            Util.Escape(STR_LOCATION),
					                            Util.Escape(((FileSystemVolumeItem)item).Location),
					                            Util.Escape(STR_VOLUME),
					                            volTitle,
					                            Util.Escape(STR_ARCHIVENO),
					                            archiveNo);
						break;
					case VolumeItemType.AudioTrackVolumeItem:
						description = string.Format("<b>{0}</b>\n<span size=\"smaller\"><i>{1}</i>\n<i>{2}:</i> {3}, <i>{4}:</i> {5}</span>",
					                            itemName,
					                            Util.Escape(STR_AUDIOTRACK),
					                            Util.Escape(STR_VOLUME),
					                            volTitle,
					                            Util.Escape(STR_ARCHIVENO),
					                            archiveNo);
						break;
					default:
						throw new NotImplementedException("Search result view has not been implemented for this volumetype");
				}
								
				store.AppendValues(GetImage(item, db),
				                   description,
				                   item);
			}
			
			this.Model = store;
			ColumnsAutosize();			
		}
		
		public void Clear() {
			if (Model != null) {
				ListStore store = (ListStore)Model;
				store.Clear();
			}
		}
		
		public VolumeItem GetItem(TreeIter iter) {
			VolumeItem item = (VolumeItem)Model.GetValue(iter, 2);
			return item;
		}
				
		private Gdk.Pixbuf GetImage(VolumeItem item, VolumeDatabase db) {
			Gdk.Pixbuf img = null;
			
			if (App.Settings.ShowThumbsInItemLists) {
				int sz = IconUtils.GetIconSizeVal(ICON_SIZE);
				img = PathUtil.GetThumb(item, db, sz);
			}
			
			if (img == null)
				img = itemIcons.GetIconForItem(item, ICON_SIZE);
			
			return img;
		}
	}
}