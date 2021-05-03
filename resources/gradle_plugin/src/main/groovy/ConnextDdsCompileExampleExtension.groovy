package com.github.rticommunity

import org.gradle.api.provider.Property
import org.gradle.api.provider.ListProperty

abstract class ConnextDdsBuildExampleExtension {
    abstract Property<String> getIdlFile()
    abstract ListProperty<String> getCodegenExtraArgs()

    ConnextDdsBuildExampleExtension() {
        idlFile.convention('')
    }
}
