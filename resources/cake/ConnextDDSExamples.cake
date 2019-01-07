/*******************************************************************************
* (c) 2019 Copyright, Real-Time Innovations, Inc.  All rights reserved.
* RTI grants Licensee a license to use, modify, compile, and create derivative
* works of the Software.  Licensee has the right to distribute object form only
* for use with RTI products.  The Software is provided "as is", with no
* warranty of any type, including any warranty for fitness for any purpose. RTI
* is under no obligation to maintain or support the Software.  RTI shall not be
* liable for any incidental or consequential damages arising out of the use or
* inability to use the software.
******************************************************************************/

var target = Argument ("target", "Default");
var rtiArchitecture = Argument ("ConnextDDSArchitecture", "");
var configuration = Argument("Configuration", "Release");

/*
 * Task to run Codegen and generate the the source code files and the
 * Visual Studio Solution.
 *
 * This tasks expects the variables:
 *     - NDDSHOME environment variable: set to the NDDSHOME folder
 *     - idlName: path to the IDL file to process.
 *     - rtiArchitecture: ConnextDDS architecture to build.
 *
 * All the generated files will be placed under the "src" folder. The "src"
 * folder will be created under the current working directory.
 */
Task("Codegen")
    .Does(() => {
        string nddshomeValue = Environment.GetEnvironmentVariable("NDDSHOME");

        if (string.IsNullOrEmpty(nddshomeValue)) {
            throw new Exception("NDDSHOME environment variable was not set");
        } else if (string.IsNullOrEmpty(rtiArchitecture)) {
            throw new Exception("The RTI Architecture must be provided");
        }

        DirectoryPath nddshome = new DirectoryPath(nddshomeValue);
        string rtiddsgen = "rtiddsgen";

        if (IsRunningOnWindows()) {
            rtiddsgen += ".bat";
        }

        string codegen = nddshome.Combine("bin").Combine(rtiddsgen).ToString();
        string lang = "-language C#";
        DirectoryPath cwd = Environment.CurrentDirectory;
        string example = "-example " + rtiArchitecture;

        string outputDir = cwd.Combine("src").ToString();
        if (!DirectoryExists(outputDir)) {
            CreateDirectory(outputDir);
        } else {
            CleanDirectory(outputDir);
        }
        string outputArg = "-d " + outputDir;

        string idlName = GetFiles("*idl").First().GetFilenameWithoutExtension().ToString();

        string idl = cwd.Combine(idlName + ".idl").ToString();

        string args = String.Format("{0} {1} -ppDisable {2} {3}",
                                    lang, example, idl, outputArg);

        ProcessSettings settings = new ProcessSettings { Arguments = args };

        using(var process = StartAndReturnProcess(codegen, settings)) {
            process.WaitForExit();
        }
    });

/*
 * Copy the publisher, subscriber and QoS profiles to the src folder.
 */
Task("Copy")
    .Does(() => {
        if (!DirectoryExists(outputDir)) {
            throw new Exception("The src folder doesn't exists");
        }

        DirectoryPath cwd = Environment.CurrentDirectory;
        DirectoryPath outputDir = cwd.Combine("src");
        CopyFiles(cwd.Combine("*cs").ToString(), outputDir);
        CopyFiles(cwd.Combine("USER_QOS_PROFILES.xml").ToString(), outputDir);
    });

/*
 * Build the project under the src folder.
 */
Task("Build")
    .Does(() => {
        if (configuration != "Release" && configuration != "Debug") {
            throw new Exception("The configuration argument is not valid");
        }

        DirectoryPath cwd = Environment.CurrentDirectory;
        DirectoryPath outputDir = cwd.Combine("src");
        if (!DirectoryExists(outputDir)) {
            throw new Exception("The src folder doesn't exists");
        }

        var pattern = outputDir.Combine("*sln").ToString();
        var found = GetFiles(pattern);

        if (found.Count() == 0) {
            throw new Exception("No SLN files were found");   
        }

        var sln = found.First();
        string slnName = sln.GetFilename().ToString();
        
        // MSBuild(slnName, settings =>
        //     settings.SetConfiguration(configuration));
    });

/*
 * Add the example.
 */
Task("AddConnextDDSExample")
    .IsDependentOn("Codegen")
    .IsDependentOn("Copy")
    .IsDependentOn("Build")
    .Does(() => {
    });

/*
 * Default task. It will execute the AddConnextDDSExample task.
 */
Task("Default")
    .IsDependentOn("AddConnextDDSExample")
    .Does(() => {
    });

RunTarget (target);
