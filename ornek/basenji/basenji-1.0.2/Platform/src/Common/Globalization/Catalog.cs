// Catalog.cs
// 
// Copyright (C) 2008, 2009, 2011 Patrick Ulbrich
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

// TODO : write an extension method 
// public static string _(this object o, string msgid)
// when mono 2.0 is included in major distros

using System;
using System.Resources;
using System.Reflection;
using System.Globalization;

namespace Platform.Common.Globalization
{
	public class Catalog
	{
		private CultureInfo ci;
		private ResourceManager rm;
		
		private Catalog(CultureInfo ci, ResourceManager rm) {
			this.ci = ci;
			this.rm = rm;
		}
		
		public string GetString(string msgid) {
			if (rm == null)
				return msgid;
				
			string translated = rm.GetString(msgid);
			return translated ?? msgid;
		}
		
		// returns the actual culture used
		// (may be different than the requested culture)
		public CultureInfo Culture {
			get {
				// may be null
				return ci;
			}
		}
		
		// if useFallbacksOnFailure is set, a Catolog object will be returned in every case:
		// if the catalog for the requested culture can't be created, it tries to
		// create a catalog for the parent culture. if this fails as well, it returns a catalog that 
		// returns the orginal, untranslated strings.
		public static Catalog GetCatalogForCulture(CultureInfo ci,
		                                           string resourceNamespace,
		                                           bool useFallbacksOnFailure) {
			if (ci == null)
				throw new ArgumentNullException("ci");
			
			if (resourceNamespace == null)
				throw new ArgumentNullException("resourceNamespace");
				
			Assembly asm = Assembly.GetCallingAssembly();
			ResourceManager rm;
			
			// try to set sub-language, e.g. "de_CH"
			rm = GetResourceManagerForCulture(ci.Name, asm, resourceNamespace); 
			if (rm == null ) {
				if (!useFallbacksOnFailure)
					return null;
					
				// try to set parent language, e.g. "de"
				ci = ci.Parent;
				if (ci != null) {
					rm = GetResourceManagerForCulture(ci.Name, asm, resourceNamespace);
					if (rm == null)
						ci = null;
				}
			}
			
			return new Catalog(ci, rm);
		}
		
		// if useFallbacksOnFailure is set a Catolog object will be returned in every case:
		// if the catalog for the requested culture name can't be created,
		// ir returns a catalog that returns the orginal, untranslated strings.
		public static Catalog GetCatalogForCulture(string name, 
		                                           string resourceNamespace, 
		                                           bool useFallbacksOnFailure) {
			if (name == null)
				throw new ArgumentNullException("name");
			
			if (resourceNamespace == null)
				throw new ArgumentNullException("resourceNamespace");
			
			Assembly asm = Assembly.GetCallingAssembly();
			ResourceManager rm;
			CultureInfo ci = null;
			
			rm = GetResourceManagerForCulture(name, asm, resourceNamespace);
			
			if (rm == null) {
				if (!useFallbacksOnFailure)
					return null;
			} else {
				try {
					// may fail on unsupported culture names like "sr" (serbian)
					ci = new CultureInfo(name);
				} catch {}
			}
			
			return new Catalog(ci, rm);			
		}
		                                           
		private static ResourceManager GetResourceManagerForCulture(string name,
		                                                            Assembly asm,
		                                                            string resourceNamespace) {
			string baseName = string.Format("{0}.{1}", 
			                                   resourceNamespace,
			                                   /* replace minus in sub-languages */
			                                   name.Replace('-', '_'));
			
			if (asm.GetManifestResourceInfo(baseName + ".resources") != null)
				return new ResourceManager(baseName, asm);
			else
				return null;
		}
	}

}
