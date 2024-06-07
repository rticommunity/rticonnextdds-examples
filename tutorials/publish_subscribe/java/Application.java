/*
 * (c) 2024 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 *
 *  RTI grants Licensee a license to use, modify, compile, and create derivative
 *  works of the Software.  Licensee has the right to distribute object form
 *  only for use with RTI products.  The Software is provided "as is", with no
 *  warranty of any type, including any warranty for fitness for any purpose.
 *  RTI is under no obligation to maintain or support the Software.  RTI shall
 *  not be liable for any incidental or consequential damages arising out of the
 *  use or inability to use the software.
 */

public abstract class Application {

    private boolean shutdownRequested = false;

    protected void addShutdownHook() {
        Runtime.getRuntime().addShutdownHook(new Thread() {
            public void run() {
                System.out.println("Shutting down...");
                shutdownRequested = true;
            }
        });
    }

    public boolean isShutdownRequested() {
        return shutdownRequested;
    }

}
