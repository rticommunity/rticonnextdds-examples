package com.github.rticommunity

import org.gradle.api.Plugin
import org.gradle.api.Project
import org.gradle.api.tasks.Exec
import org.gradle.api.tasks.Delete
import org.gradle.api.tasks.SourceSet
import org.gradle.internal.os.OperatingSystem
import org.gradle.api.tasks.SourceSetContainer
import com.github.rticommunity.ConnextDdsBuildExampleExtension

class ConnextDdsBuildExample implements Plugin<Project> {

    private static final String JAR_TASKNAME = "jar"

    private static final String SOURCE_DIR = "."

    private static final String CODEGEN_TASK_NAME = "codegen"

    private static final String TASK_AFTER_CODEGEN = "compileJava"

    private static final String JAVA_PLUGIN_NAME = "java"

    private static final String DELETE_TASK_NAME = "deleteGeneratedFiles"

    private static final String CLEAN_TASK_NAME = "clean"

    void apply(Project project)
    {
        def extension = project.extensions.create("ConfigureExample", ConnextDdsBuildExampleExtension)

        project.getPluginManager().apply(JAVA_PLUGIN_NAME)

        // Set source dir
        this.setSourceDir(project, SOURCE_DIR)

        // Find RTIConnextDDS
        def connextDdsPath = new File(System.getenv("NDDSHOME"))
        def libsPath = new File(connextDdsPath, "lib/java")

        // Add the flat directory to repositories
        project.getRepositories().flatDir {
            dirs libsPath
        }

        // Dependencies declaration
        project.getDependencies().add("implementation", ":nddsjava")

        // Edit the .jar MANIFEST file
        def classPath = new File(libsPath, "nddsjava.jar")
        this.configureJarManifest(project, classPath)

        // Add and configure the codegen Task and deleteGeneratedFiles Task
        def codegenRelativePath = "bin/rtiddsgen"

        if (OperatingSystem.current().isWindows())
            codegenRelativePath << ".bat"

        def codegenPath = new File(connextDdsPath, codegenRelativePath)
        this.addCodegenTask(project, codegenPath, extension)
        this.addDeleteGeneratedFilesTask(project, extension)
    }

    private void setSourceDir(Project project, String sourceDir)
    {
        def sourceSets = project.getExtensions().getByType(SourceSetContainer.class)
        def main = sourceSets.getByName(SourceSet.MAIN_SOURCE_SET_NAME)
        main.getJava().srcDir(sourceDir)
    }

    private void configureJarManifest(Project project, File classPath)
    {
        def jarTask = project.getTasks().getByName(JAR_TASKNAME)
        jarTask.getManifest().getAttributes().put(
                "Class-Path",
                classPath)
    }

    private void addCodegenTask(
            Project project,
            File codegenPath,
            ConnextDdsBuildExampleExtension extension)
    {
        def codegen = project.task(type: Exec.class, CODEGEN_TASK_NAME) {
            doFirst {
                if(extension.idlFile.get()) {
                    def codegenCall = [
                        codegenPath.toString(),
                        "-language",
                        "Java",
                        "-update",
                        "typefiles",
                        extension.idlFile.get()
                    ]

                    if (extension.codegenExtraArgs.get()) {
                        codegenCall.addAll(
                            codegenCall.size() - 1, extension.codegenExtraArgs.get())
                    }

                    commandLine codegenCall
                } else {
                    if (OperatingSystem.current().isWindows()) {
                        commandLine "cmd", "/c", "echo Skipping codegen because the .idl file was not provided"
                    } else {
                        commandLine "echo",  "Skipping codegen because the .idl file was not provided"
                    }
                }
            }
        }

        def compileJava = project.getTasksByName(TASK_AFTER_CODEGEN, false)
        compileJava[0].dependsOn(CODEGEN_TASK_NAME)
    }

    private void addDeleteGeneratedFilesTask(
            Project project,
            ConnextDdsBuildExampleExtension extension)
    {
        project.task(type: Delete.class, DELETE_TASK_NAME) {
            doFirst {
                def idlFile = extension.idlFile.get()
                def baseFileName = idlFile.tokenize(".")[0]

                def filesToDelete = [
                    "Application.java",
                    "${baseFileName}.java",
                    "${baseFileName}DataReader.java",
                    "${baseFileName}DataWriter.java",
                    "${baseFileName}Seq.java",
                    "${baseFileName}TypeCode.java",
                    "${baseFileName}TypeSupport.java"
                ]

                delete filesToDelete

                delete project.fileTree(".").matching {
                    include "alias_*.java"
                    include "enumeration*.java"
                    include "nested2*.java"
                    include "*MAX_*.java"
                }
            }
        }

        def cleanTask = project.getTasksByName(CLEAN_TASK_NAME, false)
        cleanTask[0].finalizedBy(DELETE_TASK_NAME)
    }
}
