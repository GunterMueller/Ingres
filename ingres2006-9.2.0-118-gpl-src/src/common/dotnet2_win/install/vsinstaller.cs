/*
** Copyright (c) 2004, 2006 Ingres Corporation. All Rights Reserved.
*/

using System;
using System.Collections;
using System.ComponentModel;
using System.Configuration;
using System.Configuration.Install;
using System.Xml;
using System.Xml.XPath;
//using EnvDTE;

namespace Ingres.Install
{
	/*
	** Name: vsinstall.cs
	**
	** Description:
	**	Implements the Installer class to modify the IDE and registry.
	**
	** Classes:
	**	IngresProviderInstaller	Installer class.
	**
	** History:
	**	 3-Feb-04 (thoda04)
	**	    Created.
	*/

	/// <summary>
	/// .NET Data Provider Installer.
	/// </summary>
	[RunInstaller(true)]
	public class IngresProviderInstaller :
		System.Configuration.Install.Installer
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		private const string dataTabname = "Data";
//		private const string dataTabname = "Ingres";

		private String[] dbProviderFactoriesPath = new String[]
			{ "configuration", "system.data", "DbProviderFactories" };
		private Hashtable ingresProviderSearchKeys = new Hashtable();
		private string    ingresProviderElement;
		private string ingresProviderElementNew = null;
		private string ingresProviderElementOld = null;

		public IngresProviderInstaller() : base()
		{
			// This call is required by the Designer.
			InitializeComponent();

			ingresProviderSearchKeys.Add(
				"invariant", "Ingres.Client");
			ingresProviderSearchKeys.Add(
				"type", "Ingres.Client.IngresFactory, Ingres.Client, " + 
				"Version=2.0.0.0, Culture = neutral, PublicKeyToken=7ab2d069d405ce41");
			ingresProviderElement =
				"<add name=\"Ingres Data Provider\" invariant=\"Ingres.Client\" description=\".NET Data Provider for Ingres\" type=\"Ingres.Client.IngresFactory, Ingres.Client, Version=2.0.0.0, Culture = neutral, PublicKeyToken=7ab2d069d405ce41\" />";

		}

		/// <summary>
		/// Perform the Ingres installation.
		/// </summary>
		/// <param name="stateSaver"></param>
		public override void Install( IDictionary stateSaver )
		{
			base.Install( stateSaver );

			//System.Windows.Forms.MessageBox.Show(
			//    "vsinstaller Install() called", "Debugging");

			ingresProviderElementNew = ingresProviderElement;
			ChangeMachineConfig();

			// save the old value in stateStater in case we need to rollback
			if (ingresProviderElementOld != ingresProviderElementNew)
				stateSaver["Ingres DbProvider"] = ingresProviderElementOld;

//			AddToolBoxItems();
		}

		/// <summary>
		/// Remove the Ingres installation from the machine.
		/// </summary>
		/// <param name="stateSaver"></param>
		public override void Uninstall( IDictionary stateSaver )
		{
			base.Uninstall(stateSaver);

			//System.Windows.Forms.MessageBox.Show(
			//    "vsinstaller Uninstall() called", "Debugging");
			if (stateSaver == null)
				return;

			ingresProviderElementNew = null;
			ChangeMachineConfig();
			stateSaver["Ingres DbProvider"] = ingresProviderElementOld;

//			RemoveToolBoxItems();
		}

		public override void Commit(IDictionary savedState)
		{
			base.Commit(savedState);

			//System.Windows.Forms.MessageBox.Show(
			//    "vsinstaller Commit() called", "Debugging");
		}

		/// <summary>
		/// Restore the pre-installation state of the machine.
		/// </summary>
		/// <param name="savedState">
		/// Contains the pre-installation state of the machine.</param>
		public override void Rollback(IDictionary savedState)
		{
			base.Rollback(savedState);

			//System.Windows.Forms.MessageBox.Show(
			//    "vsinstaller Rollback() called", "Debugging");

			if (savedState == null) return;  // safety check

			Hashtable rollbackState = new Hashtable(savedState);

			if (rollbackState.ContainsKey("Ingres DbProvider") == false)
				return;
			ingresProviderElementNew = savedState["Ingres DbProvider"] as String;
			ChangeMachineConfig();
		}

		public override string HelpText
		{
			get
			{
				return "Install Ingres .NET Data Provider";
			}
		}

		private void ChangeMachineConfig()
		{
			MachineConfigXmlDocument doc = new MachineConfigXmlDocument();
			doc.Load();   // load machine.config as XML document

			// locate the "DbProviderFactories" element
			XPathNavigator navigator = doc.CreateNavigator(dbProviderFactoriesPath);
			if (navigator == null)
				return;  // something is very wrong if we can find the DB Providers

			// locate the old <add name="Ingres Data Provider" ... />
			XPathNavigator providerNavigator =
				FindChildElement(navigator, ingresProviderSearchKeys);
			if (providerNavigator != null)
			{
				ingresProviderElementOld = providerNavigator.OuterXml;
				providerNavigator.DeleteSelf();  // if found, delete the old copy
			}

			// create the <add name="Ingres Data Provider" invariant="Ingres.Client" ...
			if (ingresProviderElementNew != null)
				navigator.AppendChild(ingresProviderElementNew);

			doc.Save();  // save the machine.config
		}
#if ENVDTE
		private void AddToolBoxItems()
		{
			EnvDTE.DTE dte = null;

			object oInstallPath = Context.Parameters["InstallPath"];
			if (oInstallPath == null)
			{
				System.Windows.Forms.MessageBox.Show(
					"InstallPath is null.  Exiting...", "Debugging");
				return;
			}
			string installPath = (string)oInstallPath;
			string dllPath =
				System.IO.Path.Combine(installPath, "Ingres.Client.dll");
			System.Windows.Forms.MessageBox.Show(
				"dllPath is \"" + dllPath + "\"", "Debugging");


			dte = LoadDTE();  // load the root object of VS.NET
			if (dte == null)
				return;

			try
			{
				EnvDTE.ToolBoxTab dataTab = FindDataTab(dte); ;
				if (dataTab == null)
				{
					dataTab = AddDataTab(dte);
				}
//					return;

				dataTab.Activate();

				RemoveItemFromTab( dataTab, "IngresDataAdapter");
				RemoveItemFromTab( dataTab, "IngresConnection");
				RemoveItemFromTab( dataTab, "IngresCommand");

				AddItemToTab(dataTab, "IngresCommand",     dllPath);
				AddItemToTab(dataTab, "IngresConnection",  dllPath);
				AddItemToTab(dataTab, "IngresDataAdapter", dllPath);
			}
			catch (Exception ex)
			{
				System.Windows.Forms.MessageBox.Show(
					"Ingres .NET Data Provider " +
					"Install.AddToolBoxItems threw Exception" +
					ex.ToString() + ".  Continuing...",
					"Ingres .NET Data Provider Install");
			}
			finally
			{
				try 
				{
					if (dte != null)
					{
						dte.Quit();  // shut down the IDE
						System.Windows.Forms.MessageBox.Show(
							"EnvDTE Quit complete", "Debugging");
					}
				}
				catch (Exception) {}
			}
		}  // AddToolBoxItems


		/// <summary>
		/// Load the root object of the Visual Studio  automation object model.
		/// </summary>
		/// <returns></returns>
		private EnvDTE.DTE LoadDTE()
		{
			EnvDTE.DTE dte = null;

			Type dteType = Type.GetTypeFromProgID(
				"VisualStudio.DTE"    );  // version latest of VS.NET
			//	"VisualStudio.DTE.7"  );  // version 2002 of VS.NET
			//	"VisualStudio.DTE.7.1");  // version 2003 of VS.NET
			if (dteType == null)
			{
				System.Windows.Forms.MessageBox.Show(
					"No type for Visual Studio.DTE.  Exiting...", "Debugging");
				return null;
			}
			try
			{
				object objDTE = System.Activator.CreateInstance(dteType, true);
				dte = (EnvDTE.DTE)objDTE;
			}
			catch (Exception ex)
			{
				System.Windows.Forms.MessageBox.Show(
					"CreateInstance(EnvDTE) threw Exception" +
					ex.ToString() + ".  Exiting...", "Debugging");
				return null;
			}

			// Work around a VS.NET bug when installing toolbox icons
			// by making the properties window visible
			if (dte != null)  // work around
				dte.ExecuteCommand("View.PropertiesWindow", String.Empty);
			return dte;
		}


		private EnvDTE.ToolBoxTab AddDataTab(EnvDTE.DTE dte)
		{
			if (dte == null)
				return null;

			//get the list of toolbox tabs
			EnvDTE.Window win = dte.Windows.Item(EnvDTE.Constants.vsWindowKindToolbox);
			EnvDTE.ToolBox toolbox = (EnvDTE.ToolBox)win.Object;          // toolbox root
			EnvDTE.ToolBoxTabs toolboxtabs = toolbox.ToolBoxTabs;  // toolbox tabs

			EnvDTE.ToolBoxTab dataTab = toolboxtabs.Add(dataTabname);
			return dataTab;
		}


		private EnvDTE.ToolBoxTab FindDataTab(EnvDTE.DTE dte)
		{
			EnvDTE.ToolBoxTab dataTab = null;
			if (dte == null)
				return null;

			try
			{
				//get the list of toolbox tabs
				EnvDTE.Window win = dte.Windows.Item(EnvDTE.Constants.vsWindowKindToolbox);
				EnvDTE.ToolBox toolbox = (EnvDTE.ToolBox)win.Object;          // toolbox root
				EnvDTE.ToolBoxTabs toolboxtabs = toolbox.ToolBoxTabs;  // toolbox tabs

				//locate the target toolbox tab
				foreach (EnvDTE.ToolBoxTab tab in toolboxtabs)
				{
					if (tab.Name == dataTabname)
					{
						dataTab = tab;
						break;
					}
				}  // end foreach
			}
			catch (Exception ex)
			{
				System.Windows.Forms.MessageBox.Show(
					"Walking toolbox threw Exception" +
					ex.ToString() + ".  Exiting...", "Debugging");
			}

			if (dataTab != null)
				System.Windows.Forms.MessageBox.Show(
					dataTab.Name + " tab located OK!", "Debugging");

			return dataTab;
		}  // FindDataTab


		private void AddItemToTab(EnvDTE.ToolBoxTab tab, string name, string dllpath)
		{
			tab.ToolBoxItems.Add(
				name,
				"Ingres.Client." + name + "," + dllpath,
				EnvDTE.vsToolBoxItemFormat.vsToolBoxItemFormatDotNETComponent);
			System.Windows.Forms.MessageBox.Show(
				name + " was added OK!", "Debugging");
		}


		private void RemoveToolBoxItems()
		{
			EnvDTE.DTE dte = null;

			dte = LoadDTE();  // load the root object of VS.NET
			if (dte == null)
				return;

			try
			{
				EnvDTE.ToolBoxTab dataTab = FindDataTab(dte); ;
				if (dataTab == null)
					return;

				// only delete Ingres toolbox tabs, don't delete "Data" tab
				if (dataTab.Name == "Advantage Ingres"  ||
					dataTab.Name == "Ingres")
				{
					System.Windows.Forms.MessageBox.Show(
						"Deleted toolbox tab "+ dataTab.Name, "Debugging");
					dataTab.Delete();
					return;
				}

				dataTab.Activate();

				RemoveItemFromTab( dataTab, "IngresDataAdapter");
				RemoveItemFromTab( dataTab, "IngresConnection");
				RemoveItemFromTab( dataTab, "IngresCommand");
			}
			catch (Exception ex)
			{
				System.Windows.Forms.MessageBox.Show(
					"RemoveToolBoxItems threw Exception" +
					ex.ToString() + ".  Exiting...", "Debugging");
			}
			finally
			{
				try 
				{
					if (dte != null)
					{
						dte.Quit();  // shut down the IDE
						System.Windows.Forms.MessageBox.Show(
							"EnvDTE Quit complete", "Debugging");
					}
				}
				catch (Exception) {}
			}
		}  // RemoveToolBoxItems


		private void RemoveItemFromTab(EnvDTE.ToolBoxTab tab, string name)
		{
			foreach (EnvDTE.ToolBoxItem item in tab.ToolBoxItems)
			{
				if (item.Name == name)
				{
					item.Delete();
					System.Windows.Forms.MessageBox.Show(
						name + " was deleted OK!", "Debugging");
				}
			}
		}
#endif  // ENVDTE

		private XPathNavigator FindChildElement(XPathNavigator navigator, IDictionary searchKeys)
		{
			XPathNodeIterator nodes =
				navigator.SelectDescendants(XPathNodeType.Element, false);

			foreach (XPathNavigator node in nodes)
			{
				if (node.HasAttributes == false) continue;  // skip empty ones

				Hashtable attributes = new Hashtable();  // attribute list

				XPathNavigator attrNavigator = node.Clone();
				bool flagMoreAttributes = attrNavigator.MoveToFirstAttribute();
				while (flagMoreAttributes)
				{
					attributes.Add(attrNavigator.Name, attrNavigator.Value);
					flagMoreAttributes = attrNavigator.MoveToNextAttribute();
				}

				bool foundit = true;
				foreach (Object searchKeyObj in searchKeys)
				{
					DictionaryEntry searchEntry = (DictionaryEntry)searchKeyObj;
					string searchKey   = searchEntry.Key.ToString();
					string searchValue = searchEntry.Value.ToString();

					// Match search keys against the child's attributes
					if (attributes.ContainsKey(searchKey) &&
						attributes[searchKey].ToString() == searchValue)
							continue;  // so far so good.  matching so far
					foundit = false;  // no match
					break;
				}

				if (foundit)
					return node;  // return the navigator to found child
			}  // foreach through the element children, still looking
			return null;  // child element not found using the keys
		}

		class MachineConfigXmlDocument : XmlDocument
		{
			public string machineConfigFilename;  // full path of machine.config

			public MachineConfigXmlDocument(): base()
			{
				// Gets the full path and filename of machine.config.
				// Usually in the example form of
				//    C:\WINDOWS\Microsoft.NET\Framework\v2.0.nnnnn\machine.config
				ConfigurationFileMap fileMap = new ConfigurationFileMap();
				machineConfigFilename = fileMap.MachineConfigFilename;
			}

			public void Load()
			{
				base.Load(machineConfigFilename);
			}

			public void Save()
			{
				base.Save(machineConfigFilename);
			}

			public XPathNavigator CreateNavigator(String[] path)
			{
				XPathNavigator navigator = this.CreateNavigator();
				string nameSpace = navigator.NamespaceURI;

				foreach (string name in path)
				{
				if (navigator.MoveToChild(name, nameSpace) == false)
					return null;  // return null if path not found
				}

				return navigator;
			}
		}  // class MachineConfigXmlDocument

		#region Component Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			components = new System.ComponentModel.Container();
		}
		#endregion
	}
}
