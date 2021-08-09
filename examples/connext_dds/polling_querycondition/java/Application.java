/*
 * (c) Copyright, Real-Time Innovations, 2020.  All rights reserved.
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the software solely for use with RTI Connext DDS. Licensee may
 * redistribute copies of the software provided that all such copies are subject
 * to this license. The software is provided "as is", with no warranty of any
 * type, including any warranty for fitness for any purpose. RTI is under no
 * obligation to maintain or support the software. RTI shall not be liable for
 * any incidental or consequential damages arising out of the use or inability
 * to use the software.
 */

import com.rti.ndds.config.LogVerbosity;

/**
 *
 * Code in the example application to handle clean shutdown, arguments, etc.
 *
 */
public abstract class Application {
    public enum ParseReturn { FAILURE, SUCCESS, EXIT }

    private boolean shutdownRequested = false;
    private int domainId = 0;
    private int maxSampleCount = Integer.MAX_VALUE;

    public void parseArguments(String[] args)
    {
        final int defaultVerbosity = 1;  // default to error
        int verbosity = defaultVerbosity;
        int argProcessing = 0;
        boolean showUsage = false;
        ParseReturn parseResult = ParseReturn.SUCCESS;

        while (argProcessing < args.length) {
            String argument = args[argProcessing];
            if ((args.length > argProcessing + 1)
                && (argument.equals("-d") || argument.equals("--domain"))) {
                domainId = Integer.parseInt(args[argProcessing + 1]);
                argProcessing++;
            } else if (
                    (args.length > argProcessing + 1)
                    && (argument.equals("-s")
                        || argument.equals("--sample-count"))) {
                maxSampleCount = Integer.parseInt(args[argProcessing + 1]);
                argProcessing++;
            } else if (
                    (args.length > argProcessing + 1)
                    && (argument.equals("-v")
                        || argument.equals("--verbosity"))) {
                verbosity = Integer.parseInt(args[argProcessing + 1]);
                argProcessing++;
            } else if (argument.equals("-h") || argument.equals("--help")) {
                System.out.println(
                        getClass().getSimpleName() + " application.");
                showUsage = true;
                parseResult = ParseReturn.EXIT;
                break;
            } else {
                System.err.println("Bad parameter: " + argument);
                showUsage = true;
                parseResult = ParseReturn.FAILURE;
                break;
            }
            argProcessing++;
        }

        final String NL = System.lineSeparator();
        if (showUsage) {
            System.out.println("Usage:" + NL
            + "    -d, --domain       <int>   DDS domain ID." + NL
            + "                               Default: 0" + NL
            + "    -s, --sample_count <int>   Number of samples to send/receive." + NL
            + "                               Default: Integer.MAX_VALUE" + NL
            + "    -v, --verbosity    <int>   Debug output level (0-3)" + NL
            + "                               Default: " + defaultVerbosity);
        }

        if (parseResult == ParseReturn.EXIT) {
            // Help requested, so exit instead of running example
            System.exit(0);
        }
        if (parseResult == ParseReturn.FAILURE) {
            throw new IllegalArgumentException();
        }
        // Always specify the verbosity, even if default
        setVerbosity(verbosity);
    }

    protected void addShutdownHook()
    {
        Runtime.getRuntime().addShutdownHook(new Thread() {
            public void run()
            {
                System.out.println("Shutting down...");
                shutdownRequested = true;
            }
        });
    }

    public boolean isShutdownRequested()
    {
        return shutdownRequested;
    }

    public int getDomainId()
    {
        return domainId;
    }

    public int getMaxSampleCount()
    {
        return maxSampleCount;
    }

    /**
     * Sets Connext verbosity to help debugging
     * @param verbosity 0 = silent, 1 = error, 2 = warning, 3 = status all.
     */
    public static void setVerbosity(int verbosity)
    {
        LogVerbosity logVerbosity;
        switch (verbosity) {
        case 0:
            logVerbosity = LogVerbosity.NDDS_CONFIG_LOG_VERBOSITY_SILENT;
            break;
        case 1:
            logVerbosity = LogVerbosity.NDDS_CONFIG_LOG_VERBOSITY_ERROR;
            break;
        case 2:
            logVerbosity = LogVerbosity.NDDS_CONFIG_LOG_VERBOSITY_WARNING;
            break;
        case 3:
            logVerbosity = LogVerbosity.NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL;
            break;
        default:
            logVerbosity = LogVerbosity.NDDS_CONFIG_LOG_VERBOSITY_ERROR;
            break;
        }
        com.rti.ndds.config.Logger.get_instance().set_verbosity(logVerbosity);
    }
}