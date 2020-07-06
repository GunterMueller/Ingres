.NET Data Provider Strong Name Keys

The Ingres .NET Data provider consists of three strong-named assemblies:

Ca.Ingres.Client
Ca.Ingres.VSDesign
Ca.Ingres.Support

Strong-named assemblies must be signed with a Strong Name Key (SNK).  In order to preserve security, CA does not distribute an SNK file.  If you wish to build the Ingres .NET from source then you must create an SNK before the first build.  To create an SNK, locate the Microsoft .NET Strong Name Tool “sn.exe”. The “sn.exe” tool is usually in the "bin" directory of Visual Studio .NET.
For example:
	C:\Program Files\Microsoft Visual Studio .NET 2003\SDK\v1.1\Bin.

Run the Strong Name Tool to create the .snk file:

	sn -k  keypair.snk

Keep a copy of the keypair.snk file in a safe, secure location as if safeguarding the keys to the data provider assemblies.  This location should be kept separate from the Ingres source tree directories to prevent accidental loss of the file in the event that the Ingres source tree is deleted and repopulated with a new source distribution.  If this file is lost and the data provider assemblies need to be rebuilt and signed with a new SNK file, all applications compiled against the old data provider that was signed with the old key signature will need to be recompiled.

Copy the keypair.snk file to this directory where the Ingres build can use it:

%ING_SRC%\common\dotnet_win\specials

The assemblyinfo file in each of the three projects for the three assemblies will reference the keypair.snk file in this directory to sign the assembly.  All three assemblies must be signed with the same keypair.snk file. 

If the Ingres source tree is deleted and repopulated with a new source distribution, simply recopy the keypair.snk file from its secure location to the common\dotnet_win\specials directory.  Because the assemblies have the same Strong Name Key signature and version, applications do not have to be recompiled.
