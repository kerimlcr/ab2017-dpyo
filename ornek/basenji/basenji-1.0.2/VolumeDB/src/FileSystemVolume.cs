// FileSystemVolume.cs
// 
// Copyright (C) 2008 Patrick Ulbrich
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

namespace VolumeDB
{
	public sealed class FileSystemVolume : Volume
	{
		private long files;
		private long dirs;
		private long size;
		
		internal FileSystemVolume(VolumeDatabase database)
			: base(database, VolumeType.FileSystemVolume)
		{
			this.files	= 0L;
			this.dirs	= 0L;
			this.size	= 0L;
		}
		
		/// <summary>
		/// <para>Required by internal factory methods like AbstractVolumeScanner.CreateVolumeObject()</para>
		/// <para>Purpose :</para>
		/// <para>
		/// - guarantee that _all_ fields of this type are initialized by the caller 
		///  (in contrast to property initialization, which easily makes you miss a property [in particular if a new one was added..])
		/// </para>
		/// <para>
		/// - seperate fields of a type from fields of its base type (e.g. AbstractVolumeScanner.CreateVolumeObject() initializes all fields of a the Volume base type. 
		/// Caller code only needs to initialize fields of the derived Volume type)
		/// </para>
		/// </summary>
		internal void SetFileSystemVolumeFields(long files, long dirs, long size) {
			this.files	= files;
			this.dirs	= dirs;
			this.size	= size;			  
		}
		
		// FileSystemVolume specific implementation of Volume.GetRoot()
		public new DirectoryVolumeItem GetRoot() {
			return (DirectoryVolumeItem)base.GetRoot();
		}
		
		internal override void ReadFromVolumeDBRecord(IRecordData recordData) {
			base.ReadFromVolumeDBRecord(recordData);

			files = (long)recordData["Files"];
			dirs  = (long)recordData["Dirs"];
			size  = (long)recordData["Size"];
		}

		internal override void WriteToVolumeDBRecord(IRecordData recordData) {
			base.WriteToVolumeDBRecord(recordData);

			recordData.AddField("Files", files);
			recordData.AddField("Dirs",  dirs);
			recordData.AddField("Size",  size);
		}

		#region read-only properties
	   
		public long Files {
			get { return files; }
			internal set { files = value; }
		}

		public long Directories {
			get { return dirs; }
			internal set { dirs = value; }
		}

		public long Size {
			get { return size; }
			internal set { size = value; }
		}
		#endregion
	}
}
