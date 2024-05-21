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

/**
 * Hold information about the pipeline.
 */
Map pipelineInfo = [:]

pipeline {
    agent {
        label "${runInsideExecutor.labelFromJobName()}"
    }

    options {
        disableConcurrentBuilds()
        /*
            To avoid excessive resource usage in server, we limit the number
            of builds to keep in pull requests
        */
        buildDiscarder(
            logRotator(
                artifactDaysToKeepStr: '',
                artifactNumToKeepStr: '',
                daysToKeepStr: '',
                /*
                   For pull requests only keep the last 10 builds, for regular
                   branches keep up to 20 builds.
                */
                numToKeepStr: changeRequest() ? '10' : '20'
            )
        )
        // Set a timeout for the entire pipeline
        timeout(time: 2, unit: 'HOURS')
    }

    stages {
        stage('Configuration') {
            steps {
                script {
                    pipelineInfo.dockerDir = "${env.WORKSPACE}/resources/docker/"
                    pipelineInfo.staticAnalysisDir = "${env.WORKSPACE}/static_analysis_report"
                    runInsideExecutor(
                        '',
                        pipelineInfo.dockerDir,
                    ) {
                        pipelineInfo.connextArch = getEnvVar('CONNEXTDDS_ARCH')
                    }
                }
            }
        }
        stage('Download Packages') {
            steps {
                runInsideExecutor(
                    '',
                    pipelineInfo.dockerDir,
                ) {
                    script {
                        pipelineInfo.connextDir = installConnext(
                            pipelineInfo.connextArch,
                            env.WORKSPACE,
                        )
                    }
                }
            }
        }
        stage('Build all modes') {
            matrix {
                axes {
                    axis {
                        name 'buildMode'
                        values 'release', 'debug'
                    }
                    axis {
                        name 'linkMode'
                        values 'static', 'dynamic'
                    }
                }
                stages {
                    stage('Build single mode') {
                        steps {
                            runInsideExecutor(
                                '',
                                pipelineInfo.dockerDir,
                            ) {
                                echo("Building ${buildMode}/${linkMode}")
                                buildExamples(
                                    pipelineInfo.connextArch,
                                    pipelineInfo.connextDir,
                                    buildMode,
                                    linkMode,
                                    env.WORKSPACE,
                                )
                            }
                        }
                    }
                }
            }
        }
        stage('Static Analysis') {
            steps {
                runInsideExecutor(
                    '',
                    pipelineInfo.dockerDir,
                ) {
                    runStaticAnalysis(
                        buildExamples.getBuildDirectory('release', 'dynamic'),
                        pipelineInfo.connextDir,
                        pipelineInfo.staticAnalysisDir,
                    )
                }
                publishHTML(target: [
                    allowMissing: true,
                    alwaysLinkToLastBuild: true,
                    keepAll: true,
                    reportDir: pipelineInfo.staticAnalysisDir,
                    reportFiles: 'index.html',
                    reportName: 'LLVM Scan build static analysis',
                ])
            }
        }
    }
    post {
        cleanup {
            cleanWs()
        }
    }
}

