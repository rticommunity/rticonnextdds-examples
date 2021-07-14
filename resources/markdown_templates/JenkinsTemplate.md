<!-- markdownlint-disable MD033 MD041 MD013 -->
This build is being executed on an internal Jenkins, only RTI employees can access the build logs. To give information to external users, we have configured the status checks.

# Pipeline description

The pipeline is defined [here](https://github.com/rticommunity/rticonnextdds-examples/blob/develop/Jenkinsfile) and the purpose of this pipeline is to build the examples using the last staging packages of RTI Connext® DDS and to analyze the build using `analyze-build` from `clang-tools`.

# Environment details

| Option                   | Setting               |
| ------------------------ | --------------------- |
| RTI Connext® DDS Version | @RTI_PACKAGE_VERSION@ |
| System                   | @OPERATING_SYSTEM@    |
| Machine type             | @MACHINE_TYPE@        |
| OS release               | @OS_VERSION@          |
@EXTRA_ROWS@

<details><summary>Dockerfile</summary>
<p>

```Dockerfile
@DOCKERFILE@
```

</p>
</details>

<details><summary>Jenkinsfile</summary>
<p>

```Groovy
@JENKINSFILE@
```

</p>
</details>

<details><summary>Logs</summary>
<p>

```plaintext
@LOGS@
```

</p>
</details>
