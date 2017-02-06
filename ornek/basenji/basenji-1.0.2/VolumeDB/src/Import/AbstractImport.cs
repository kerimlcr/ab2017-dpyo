// AbstractImport.cs
// 
// Copyright (C) 2010, 2012 Patrick Ulbrich
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
using System.IO;
using System.Collections.Generic;
using System.Threading;
using System.ComponentModel;
using Platform.Common.Diagnostics;

namespace VolumeDB.Import
{
	public abstract class AbstractImport : IImport
	{
		private List<string>	volumeDataPaths;
		private string			sourceDbPath;
		private VolumeDatabase	targetDb;
		private string			dbDataPath;
		private int				bufferSize;
		
		private volatile bool isRunning;
		private volatile bool importSucceeded;
		private volatile bool cancellationRequested;
		
		private double lastCompleted;
		
		private AsyncOperation asyncOperation;
		
		internal AbstractImport (string sourceDbPath,
		                         VolumeDatabase targetDb,
		                         string dbDataPath,
		                         int bufferSize) {
			
			if (sourceDbPath == null)
				throw new ArgumentNullException("sourceDbPath");
			
			if (targetDb == null)
				throw new ArgumentNullException("targetDb");
			
			if (dbDataPath == null)
				throw new ArgumentNullException("dbDataPath");
			
			this.volumeDataPaths		= new List<string>();
			this.sourceDbPath			= sourceDbPath;
			this.targetDb				= targetDb;
			this.dbDataPath				= dbDataPath;
			this.bufferSize				= bufferSize;
			
			this.isRunning				= false;
			this.importSucceeded		= false;
			this.cancellationRequested	= false;
		}
		
		public static string[] GetSupportedExtensions() {
			var lst = new List<string>();
			
			foreach (var t in GetImportTypes()) {
				var attrs = t.GetCustomAttributes(false);
				foreach(var a in attrs) {
					var ia = a as ImportAttribute;
					if (ia != null)
						lst.Add(ia.Extension);
				}
			}
			return lst.ToArray();
		}
		
		public static IImport GetImportByExtension(string extension, string sourceDbPath, 
		                                           VolumeDatabase targetDb, string dbDataPath,
		                                           int bufferSize) {
			
			if (extension == null)
				throw new ArgumentNullException("extension");
			
			foreach (var t in GetImportTypes()) {
				var attrs = t.GetCustomAttributes(false);
				foreach(var a in attrs) {
					var ia = a as ImportAttribute;
					if ((ia != null) && (ia.Extension.ToUpper() == extension.ToUpper())) {
						return (IImport)Activator.CreateInstance(t, new object[] { 
							sourceDbPath, targetDb, dbDataPath, bufferSize
						});
					}
				}
			}
			
			return null;
		}
		
		private static Type[] importTypes = null;
		private static Type[] GetImportTypes() {
			if (importTypes == null) {
				var lst = new List<Type>();
				Type[] types = typeof(AbstractImport).Assembly.GetTypes();
				
				foreach (var t in types) {
					if (t.IsSubclassOf(typeof(AbstractImport))) {
						lst.Add(t);
					}
				}
				
				importTypes = lst.ToArray();
			}
			return importTypes;
		}
		
		public WaitHandle RunAsync() {
			if (isRunning)
				throw new InvalidOperationException("Import is already running");
			
			try {
				/* must be set (as soon as possible) in a function that is _not_ called asynchronously 
				 * (i.e. dont call it in ImportThread()) */
				isRunning = true;
				importSucceeded = false;
				cancellationRequested = false;
				asyncOperation = AsyncOperationManager.CreateOperation(null);

				Reset();
				
				/* invoke the import function on a new thread and return a waithandle */
				Action<string, VolumeDatabase, string, int> it = ImportThread;
				IAsyncResult ar = it.BeginInvoke(sourceDbPath, targetDb, dbDataPath, bufferSize, null, null);
				return ar.AsyncWaitHandle;
			
			} catch (Exception) {
				isRunning = false;

				if (asyncOperation != null)
					asyncOperation.OperationCompleted();

				throw;
			}
		}
		
		public void CancelAsync() {
			cancellationRequested = true;
		}

		public bool IsBusy {
			get { return isRunning; }
		}

		public bool ImportSucceeded {
			get { return importSucceeded; }
		}
		
		public string Name {
			get {
				object[] attrs = this.GetType().GetCustomAttributes(false);
				foreach (var a in attrs) {
					var ia = a as ImportAttribute;
					if (ia != null)
						return ia.Name;
				}
				return ToString();
			}
		}
		
		#region Events
		public event ErrorEventHandler				Error;
		public event ImportCompletedEventHandler	ImportCompleted;
		public event ProgressUpdateEventHandler		ProgressUpdate;
		#endregion
		
		protected void CheckForCancellationRequest() {
			if (cancellationRequested)
				throw new ImportCancelledException();
		}
		
		protected virtual void Reset() {
			lastCompleted = .0;
			volumeDataPaths.Clear();
		}
		
		// TODO : make this member internally protected in case this language feature has become real
		// see http://lab.msdn.microsoft.com/productfeedback/viewfeedback.aspx?feedbackid=33c53cf6-2709-4cc9-a408-6cafee4313ef
		//protected
		internal
		abstract void ImportThreadMain(string sourceDbPath,
			                               VolumeDatabase targetDb, 
			                               string dbDataPath,
			                               BufferedVolumeItemWriter writer);
		
		private void ImportThread(string sourceDbPath,
		                          VolumeDatabase targetDb,
		                          string dbDataPath,
		                          int buferSize) {
			
			Exception fatalError = null;
			bool cancelled = false;
			
			try {
				// must be the first call within the try block
				targetDb.TransactionBegin(); // locks VolumeDatabase
				
				if (!File.Exists(sourceDbPath))
					throw new FileNotFoundException("Source database not found");
				
				// note: 
				// don't use the writer in a using() block here as dispose() would write
				// buffered items after an exception has been thrown.
				BufferedVolumeItemWriter writer = new BufferedVolumeItemWriter(targetDb, true, bufferSize);
				
				ImportThreadMain(sourceDbPath,
				                 targetDb,
				                 dbDataPath,
				                 writer);
				
				writer.Close();
				
				targetDb.TransactionCommit(); // unlocks VolumeDatabase
				importSucceeded = true;
			
			} catch (Exception ex) {
				
				Exception cleanupException = null;
				try {
					targetDb.TransactionRollback();  // unlocks VolumeDatabase
					
					foreach (string path in volumeDataPaths)
						Directory.Delete(path, true);
				} catch (Exception e) {
					cleanupException = e;
				}
				
				if (ex is ImportCancelledException) {
					cancelled = true;
				} else {
					/* save the error that caused the import to stop (import failure) */
					fatalError = ex;
					PostError(ex);
					
					Debug.WriteLine("Details for exception in ImportThread():\n" + ex.ToString());
				}

				// in case an error occured while cleaning up, 
				// post the error here, _after_ the initial error that made the import fail.
				if (cleanupException != null) {
					PostError(cleanupException);
				}
				
//#if THROW_EXCEPTIONS_ON_ALL_THREADS
//				  if (!(ex is ScanCancelledException))
//					  throw;
//#endif
			} finally {
				isRunning = false;
				PostCompleted(fatalError, cancelled);
			}
		}
		
		protected string CreateThumbsDir(string dbDataPath, long volumeID) {
			string volumeDataPath = DbData.CreateVolumeDataPath(dbDataPath, volumeID);
			volumeDataPaths.Add(volumeDataPath);
			return DbData.CreateVolumeDataThumbsPath(volumeDataPath);
		}
		
		protected static void ImportThumb(long sourceID,
		                                  long targetID,
		                                  string sourceThumbsPath,
		                                  string targetThumbsPath) {
			
			string sourceThumb = Path.Combine(sourceThumbsPath, sourceID.ToString() + ".png");
			string targetThumb = Path.Combine(targetThumbsPath, targetID.ToString() + ".png");
			
			if (File.Exists(sourceThumb))
				File.Copy(sourceThumb, targetThumb);
		}
		
		protected void PostProgressUpdate(double completed) {
			// update progress on every full percent point only
			// to save resources and cpu
			if (((int)completed - (int)lastCompleted) < 1)
				return;
			
			lastCompleted = completed;
			
			SendOrPostCallback cb = delegate(object args) {
				OnProgressUpdate((ProgressUpdateEventArgs)args);
			};

			ProgressUpdateEventArgs e = new ProgressUpdateEventArgs(completed);
			asyncOperation.Post(cb, e);
		}
		
		/// <summary>
		/// Called when an unhandled exception occurs.
		/// </summary>
		private void PostError(Exception ex) {
			SendOrPostCallback cb = delegate(object args) {
				OnError((VolumeDB.Import.ErrorEventArgs)args);
			};

			ErrorEventArgs e = new ErrorEventArgs(ex);
			asyncOperation.Post(cb, e);
		}
		
		/// <summary>
		/// Called when importing has been completed.
		/// </summary>
		private void PostCompleted(Exception error, bool cancelled) {
			SendOrPostCallback cb = delegate(object args) {
				OnImportCompleted((ImportCompletedEventArgs)args);
			};

			ImportCompletedEventArgs e = new ImportCompletedEventArgs(error, cancelled);
			asyncOperation.PostOperationCompleted(cb, e);
		}
		
		private void OnProgressUpdate(ProgressUpdateEventArgs e) {
			if (this.ProgressUpdate != null)
				this.ProgressUpdate(this, e);
		}
		
		private void OnError(ErrorEventArgs e) {
			if (this.Error != null)
				this.Error(this, e);
		}

		private void OnImportCompleted(ImportCompletedEventArgs e) {
			if (this.ImportCompleted != null)
				this.ImportCompleted(this, e);
		}
		
		/// <summary>
		/// Signals that the import has been cancelled.
		/// </summary>
		protected class ImportCancelledException : Exception {
			public ImportCancelledException() : base() { }
		}
	}
}
