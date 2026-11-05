/*
 * (c) Copyright, Real-Time Innovations, 2026.  All rights reserved.
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the software solely for use with RTI Connext DDS. Licensee may
 * redistribute copies of the software provided that all such copies are subject
 * to this license. The software is provided "as is", with no warranty of any
 * type, including any warranty for fitness for any purpose. RTI is under no
 * obligation to maintain or support the software. RTI shall not be liable for
 * any incidental or consequential damages arising out of the use or inability
 * to use the software.
 */

package com.rti.dl.adapters;

import java.io.Serializable;

import com.rti.dl.DistLogger;
import com.rti.dl.Options;

import org.apache.logging.log4j.Level;
import org.apache.logging.log4j.core.Core;
import org.apache.logging.log4j.core.Layout;
import org.apache.logging.log4j.core.LogEvent;
import org.apache.logging.log4j.core.StringLayout;
import org.apache.logging.log4j.core.appender.AbstractAppender;
import org.apache.logging.log4j.core.config.plugins.Plugin;
import org.apache.logging.log4j.core.config.plugins.PluginBuilderAttribute;
import org.apache.logging.log4j.core.config.plugins.PluginBuilderFactory;

/**
 * @brief Sends Log4j events to RTI Distributed Logger.
 *
 * The layout creates the message text, and the Log4j logger name becomes the
 * DDS category. Log4j discovers this appender through its generated plugin
 * metadata under the name DDSAppender.
 *
 * All instances share the same Distributed Logger singleton. Stopping an
 * appender does not delete that singleton. The application must stop logging,
 * shut down Log4j, and then call DistLogger.delete().
 */
@Plugin(name = "DDSAppender", category = Core.CATEGORY_NAME)
public final class DDSAppender extends AbstractAppender {
    private static final int SILENT_LEVEL = 0;
    private static final int FATAL_LEVEL = 100;
    private static final int ERROR_LEVEL = 300;
    private static final int WARNING_LEVEL = 400;
    private static final int INFO_LEVEL = 600;
    private static final int DEBUG_LEVEL = 700;
    private static final int TRACE_LEVEL = 800;

    private final StringLayout stringLayout;
    private final boolean configureDistLogger;
    private final int domainId;
    private final String applicationKind;
    private final String qosLibrary;
    private final String qosProfile;
    private final Level filterLevel;
    private volatile DistLogger distLogger;

    private DDSAppender(Builder builder, StringLayout layout) {
        super(builder.getName(), builder.getFilter(), layout,
                builder.isIgnoreExceptions(), builder.getPropertyArray());
        stringLayout = layout;
        configureDistLogger = builder.configureDistLogger;
        domainId = builder.domainId;
        applicationKind = builder.applicationKind;
        qosLibrary = builder.qosLibrary;
        qosProfile = builder.qosProfile;
        filterLevel = builder.filterLevel;
    }

    /**
     * @brief Creates a builder for a DDS appender.
     *
     * @return A new builder with the default settings.
     */
    @PluginBuilderFactory
    public static Builder newBuilder() {
        return new Builder();
    }

    /**
     * @brief Collects and checks the settings used to create an appender.
     *
     * By default, the appender configures Distributed Logger for domain 0,
     * an empty application kind, and an INFO filter level. The inherited
     * builder settings provide the name, layout, filter, and error handling.
     */
    public static final class Builder extends AbstractAppender.Builder<Builder>
            implements org.apache.logging.log4j.core.util.Builder<DDSAppender> {
        @PluginBuilderAttribute
        private boolean configureDistLogger = true;

        @PluginBuilderAttribute
        private int domainId = 0;

        @PluginBuilderAttribute
        private String applicationKind = "";

        @PluginBuilderAttribute
        private String qosLibrary;

        @PluginBuilderAttribute
        private String qosProfile;

        @PluginBuilderAttribute
        private Level filterLevel = Level.INFO;

        /**
         * @brief Chooses whether the appender sets Distributed Logger options.
         *
         * @param configureDistLogger True to set options before creating the
         * singleton; false to use the current or default options. The default
         * is true. When false, the domain, application kind, QoS, and filter
         * settings in this builder are checked but not applied.
         * @return This builder.
         */
        public Builder setConfigureDistLogger(boolean configureDistLogger) {
            this.configureDistLogger = configureDistLogger;
            return this;
        }

        /**
         * @brief Sets the DDS domain used by Distributed Logger.
         *
         * @param domainId A nonnegative domain ID. The default is 0.
         * @return This builder.
         */
        public Builder setDomainId(int domainId) {
            this.domainId = domainId;
            return this;
        }

        /**
         * @brief Sets the application identity shown by Distributed Logger.
         *
         * @param applicationKind A string of at most 255 characters.
         * An empty string is allowed and is the default. Do not pass null.
         * @return This builder.
         */
        public Builder setApplicationKind(String applicationKind) {
            this.applicationKind = applicationKind;
            return this;
        }

        /**
         * @brief Selects an existing DDS QoS library.
         *
         * @param qosLibrary A library name that is not empty or made up only
         * of spaces. Pass null to leave it unset.
         * A library and profile must be supplied together.
         * @return This builder.
         */
        public Builder setQosLibrary(String qosLibrary) {
            this.qosLibrary = qosLibrary;
            return this;
        }

        /**
         * @brief Selects a profile from the chosen DDS QoS library.
         *
         * @param qosProfile A profile name that is not empty or made up only
         * of spaces. Pass null to leave it unset.
         * A library and profile must be supplied together.
         * @return This builder.
         */
        public Builder setQosProfile(String qosProfile) {
            this.qosProfile = qosProfile;
            return this;
        }

        /**
         * @brief Sets the initial filter level for Distributed Logger.
         *
         * This filter is separate from Log4j's logger levels and filters.
         * Both must allow an event for it to be forwarded.
         *
         * @param filterLevel A Log4j level, such as INFO or DEBUG.
         * The default is INFO. Do not pass null.
         * @return This builder.
         */
        public Builder setFilterLevel(Level filterLevel) {
            this.filterLevel = filterLevel;
            return this;
        }

        /**
         * @brief Checks the settings and creates an appender without starting it.
         *
         * A name and a text layout are required. The name must not be null,
         * empty, or made up only of spaces. If no layout is supplied, the
         * inherited builder creates a default PatternLayout.
         * Invalid settings are reported through Log4j's status logger.
         *
         * @return The new appender, or null if a setting is invalid.
         */
        @Override
        public DDSAppender build() {
            if (getName() == null || getName().trim().isEmpty()) {
                LOGGER.error("DDSAppender requires a nonempty name");
                return null;
            }
            if (domainId < 0) {
                LOGGER.error("DDSAppender domainId must be nonnegative");
                return null;
            }
            if (applicationKind == null || applicationKind.length() > 255) {
                LOGGER.error("DDSAppender applicationKind must be nonnull and at most 255 characters");
                return null;
            }
            if ((qosLibrary == null) != (qosProfile == null)
                    || (qosLibrary != null && (qosLibrary.trim().isEmpty()
                            || qosProfile.trim().isEmpty()))) {
                LOGGER.error("DDSAppender requires both qosLibrary and qosProfile, or neither");
                return null;
            }
            if (filterLevel == null) {
                LOGGER.error("DDSAppender requires a filterLevel");
                return null;
            }
            Layout<? extends Serializable> layout = getOrCreateLayout();
            if (!(layout instanceof StringLayout)) {
                LOGGER.error("DDSAppender requires a StringLayout, such as PatternLayout");
                return null;
            }
            return new DDSAppender(this, (StringLayout) layout);
        }
    }

    /**
     * @brief Initializes Distributed Logger and starts this appender.
     *
     * If configuration is enabled, options must be set before any code creates
     * the shared singleton. An existing singleton causes a configuration error.
     * If configuration is disabled, this method uses the existing singleton
     * or creates it with the current or default options.
     *
     * Calling this method on an already started appender has no effect.
     * Startup failures are reported through the appender's error handler.
     */
    @Override
    public synchronized void start() {
        if (isStarted()) {
            return;
        }
        try {
            synchronized (DistLogger.class) {
                if (configureDistLogger) {
                    Options options = new Options();
                    options.setDomainId(domainId);
                    options.setApplicationKind(applicationKind);
                    options.setFilterLevel(toDistLoggerLevel(filterLevel));
                    if (qosLibrary != null) {
                        options.setQosLibrary(qosLibrary);
                        options.setQosProfile(qosProfile);
                    }
                    if (!DistLogger.setOptions(options)) {
                        throw new IllegalStateException(
                                "Distributed Logger is already initialized; "
                                + "use configureDistLogger=false to reuse it");
                    }
                }
                distLogger = DistLogger.getInstance();
            }
            super.start();
        } catch (RuntimeException | LinkageError failure) {
            error("Unable to start DDSAppender '" + getName() + "'", failure);
        }
    }

    /**
     * @brief Formats an event and adds it to the Distributed Logger queue.
     *
     * Events at OFF are skipped. The layout supplies the message text,
     * including any formatted exception, and the logger name supplies the
     * category. Adding an event to the queue does not guarantee DDS delivery.
     * Calls before startup and forwarding failures are reported through the
     * appender's error handler.
     *
     * @param event The Log4j event to forward. Do not pass null.
     */
    @Override
    public void append(LogEvent event) {
        DistLogger logger = distLogger;
        if (!isStarted() || logger == null) {
            error("DDSAppender '" + getName() + "' is not started", event,
                    new IllegalStateException("DDSAppender is not started"));
            return;
        }
        try {
            int level = toDistLoggerLevel(event.getLevel());
            if (level == SILENT_LEVEL) {
                return;
            }
            String message = stringLayout.toSerializable(event);
            logger.log(level, message, event.getLoggerName());
        } catch (RuntimeException failure) {
            error("Unable to forward event from DDSAppender '" + getName() + "'",
                    event, failure);
        }
    }

    /**
     * @brief Converts a Log4j level to a Distributed Logger numeric level.
     *
     * Custom levels use their Log4j standard-level group. OFF maps to SILENT,
     * WARN maps to WARNING, and both ALL and TRACE map to TRACE.
     *
     * @param level The Log4j level to convert. Do not pass null.
     * @return The matching Distributed Logger level.
     * @throws IllegalArgumentException If the standard level is unsupported.
     */
    private static int toDistLoggerLevel(Level level) {
        switch (level.getStandardLevel()) {
            case OFF:
                return SILENT_LEVEL;
            case FATAL:
                return FATAL_LEVEL;
            case ERROR:
                return ERROR_LEVEL;
            case WARN:
                return WARNING_LEVEL;
            case INFO:
                return INFO_LEVEL;
            case DEBUG:
                return DEBUG_LEVEL;
            case TRACE:
            case ALL:
                return TRACE_LEVEL;
            default:
                throw new IllegalArgumentException("Unsupported Log4j level: " + level);
        }
    }
}