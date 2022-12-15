/*
 * (c) Copyright, Real-Time Innovations, 2021.  All rights reserved.
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the software solely for use with RTI Connext DDS. Licensee may
 * redistribute copies of the software provided that all such copies are subject
 * to this license. The software is provided "as is", with no warranty of any
 * type, including any warranty for fitness for any purpose. RTI is under no
 * obligation to maintain or support the software. RTI shall not be liable for
 * any incidental or consequential damages arising out of the use or inability
 * to use the software.
 */

package com.github.rticommunity

import org.gradle.api.Plugin
import org.gradle.api.Project
import org.gradle.api.tasks.Exec
import org.gradle.api.tasks.Delete
import org.gradle.api.tasks.SourceSet
import org.gradle.api.plugins.JavaApplication
import org.gradle.internal.os.OperatingSystem
import org.gradle.api.tasks.SourceSetContainer
import com.github.rticommunity.ConnextDdsBuildExampleExtension

/**@
 *  This class contains a plugin implementation for building the Java Examples
 */
class ConnextDdsBuildExample implements Plugin<Project> {
    /** the path of the source directory in the example directory */
    private static final String SOURCE_DIR = "."

    /** the name of the path that executes codegen */
    private static final String CODEGEN_TASK_NAME = "codegen"

    /** the name of the task that compiles the java files */
    private static final String TASK_AFTER_CODEGEN = "compileJava"

    /** the name of the java plugin (build the class and jar files) */
    private static final String JAVA_PLUGIN_NAME = "java"

    /** the name of the application plugin (runs the class and jar files) */
    private static final String APPLICATION_PLUGIN_NAME = "application"

    /** the name of the task for deleting codegen files */
    private static final String DELETE_TASK_NAME = "deleteGeneratedFiles"

    /** the name of the task that cleans the build directory */
    private static final String CLEAN_TASK_NAME = "clean"

    /** the name of the task that configures the application plugin */
    private static final String CONFIGURERUN_TASK_NAME = "configureRun"

    /** the name of the run task of the application plugin */
    private static final String RUN_TASK_NAME = "run"

    /**@
     * Applies all the plugin configuration
     *
     * @param project the gradle's project object
     */
    void apply(Project project)
    {
        /** 
         * Creates the extension to get the idl's file name and codegen extra
         * args.
         * 
         * ConfigureExample {
         *   idlFile = "<idlFileName>"
         *   codegenExtraArgs = [arg1, arg2, ..., argN]  
         * }
        */
        def extension = 
            project
            .extensions
            .create("ConfigureExample", ConnextDdsBuildExampleExtension)

        /** Applies the java plugin */
        project.getPluginManager().apply(JAVA_PLUGIN_NAME)

        /** Sets the source dir */
        this.setSourceDir(project, SOURCE_DIR)

        /** Finds RTIConnextDDS using the env var NDDSHOME */
        def connextDdsPath = new File(System.getenv("NDDSHOME"))
        def libsPath = new File(connextDdsPath, "lib/java")

        /** Adds the directory to find the RTIConnextDDS Java libraries */
        project.getRepositories().flatDir {
            dirs libsPath
        }

        /** Adds the dependency at compile and run time */
        project.getDependencies().add("implementation", ":nddsjava")

        /** Adds and configures the created tasks */
        this.addCodegenTask(project, extension)
        this.addDeleteGeneratedFilesTask(project, extension)
        this.addAndConfigureApplicationPlugin(project, extension)
    }

    /**@
     * Sets the source directory with the path which contains the .java files.
     *
     * @param project the gradle's project object
     * @param sourceDir the source directory to set
     */
    private void setSourceDir(Project project, String sourceDir)
    {
        def sourceSets = project
            .getExtensions()
            .getByType(SourceSetContainer.class)
        def main = sourceSets.getByName(SourceSet.MAIN_SOURCE_SET_NAME)
        main.getJava().srcDir(sourceDir)
    }

    /**@
     * Adds the codegen task to the project. This task calls rtiddsgen to
     * generate all the required source files for building the examples.
     *
     * @param project the gradle's project object
     * @param extension contains the name of the idlFile and the codegen extra
     * args
     */
    private void addCodegenTask(
            Project project,
            ConnextDdsBuildExampleExtension extension)
    {
        /** Adds the codegen task */
        def codegen = project.task(CODEGEN_TASK_NAME) {
            doFirst {
                /** There are a few examples without .idl file */
                if(extension.idlFile.get()) {
                    def codegenExecutableName = "rtiddsgen"

                    def codegenArgs = [
                        "-language",
                        "Java",
                        "-update",
                        "typefiles",
                        extension.idlFile.get()
                    ]

                    if (OperatingSystem.current().isWindows()) {
                        codegenExecutableName += ".bat"
                    }

                    /** inserts the extra args if it was given */
                    if (extension.codegenExtraArgs.get()) {
                        codegenArgs.addAll(
                            codegenArgs.size() - 1,
                            extension.codegenExtraArgs.get())
                    }

                    /** execs rtiddsgen */
                    project.exec {
                        executable codegenExecutableName
                        args codegenArgs
                    }
                } else {
                    println "Skipping codegen because the .idl file was not provided"
                }
            }
        }

        /**
         * The codegen task should be called after compileJava because
         * rtiddsgen generates files that are necessary at compile time.
         */
        def compileJava = project.getTasksByName(TASK_AFTER_CODEGEN, false)
        compileJava[0].dependsOn(CODEGEN_TASK_NAME)
    }

    /**@
     * Adds the deleteGeneratedFiles task to the project. This task deletes
     * the rtiddsgen's generated files.
     *
     * @param project the gradle's project object
     * @param extension contains the name of the idlFile and the codegen extra
     * args
     */
    private void addDeleteGeneratedFilesTask(
            Project project,
            ConnextDdsBuildExampleExtension extension)
    {
        /** Adds deleteGeneratedFiles task to the project */
        project.task(type: Delete.class, DELETE_TASK_NAME) {
            doFirst {
                def idlFile = extension.idlFile.get()
                def baseFileName = idlFile.tokenize(".")[0]

                /** The list of files to delete */
                def filesToDelete = [
                    "${baseFileName}.java",
                    "${baseFileName}DataReader.java",
                    "${baseFileName}DataWriter.java",
                    "${baseFileName}Seq.java",
                    "${baseFileName}TypeCode.java",
                    "${baseFileName}TypeSupport.java"
                ]

                delete filesToDelete

                /** Deletes another generated files using regular expressions */
                delete project.fileTree(".").matching {
                    include "alias_*.java"
                    include "enumeration*.java"
                    include "nested2*.java"
                    include "*MAX_*.java"
                }
            }
        }

        /**
         * The clean task deletes the build directory, so they make a
         * good team.
         */
        def cleanTask = project.getTasksByName(CLEAN_TASK_NAME, false)
        cleanTask[0].finalizedBy(DELETE_TASK_NAME)
    }

    /**@
     * Adds the configureRun task to the project. Finds the publisher or the
     * subscriber and configures the application plugin to execute them.
     *
     * @param project the gradle's project object
     * @param extension contains the name of the idlFile and the codegen extra
     * args
     */
    private void addAndConfigureApplicationPlugin(
            Project project,
            ConnextDdsBuildExampleExtension extension)
    {
        /** Applies the application plugin */
        project.getPluginManager().apply(APPLICATION_PLUGIN_NAME)

        /** Adds the configureRun task */
        project.task(CONFIGURERUN_TASK_NAME) {
            doFirst {
                /** 
                 * The name of the IDL matches with the name of the publisher
                 * and with thename of the subscriber.
                */
                def idlFile = extension.idlFile.get()
                def baseName = idlFile.tokenize(".")[0]

                /** 
                 * Gets the mainClass property. If it is not defined, the null
                 * value will be assigned.
                 */
                def mainClass = project.hasProperty("mainClass") ?
                project.getProperty("mainClass").capitalize() : null

                /** If mainClass is null or has an unvalid value, it will fail */
                if (
                    mainClass == null 
                    || !["Publisher", "Subscriber"].contains(mainClass)
                ) {
                    fail(
                        "mainClass not provided, use -PmainClass=<publisher|subscriber> to provide it"
                    )
                }

                /** Gets the application's extension and sets the mainClass */
                def javaApplication = project
                    .getExtensions()
                    .getByType(JavaApplication.class)
                javaApplication.setMainClass("${baseName}${mainClass}")
            }
        }

        /** Makes configureRun task executes before run */
        def runTask = project.getTasksByName(RUN_TASK_NAME, false)
        runTask[0].dependsOn(CONFIGURERUN_TASK_NAME)
    }
}
