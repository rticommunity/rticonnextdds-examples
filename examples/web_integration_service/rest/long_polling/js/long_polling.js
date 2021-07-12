/*
 * (c) 2017 Copyright, Real-Time Innovations, Inc.  All rights reserved.
 * RTI grants Licensee a license to use, modify, compile, and create derivative
 * works of the Software.  Licensee has the right to distribute object form
 * only for use with RTI products.  The Software is provided "as is", with no
 * warranty of any type, including any warranty for fitness for any purpose.
 * RTI is under no obligation to maintain or support the Software.  RTI shall
 * not be liable for any incidental or consequential damages arising out of the
 * use or inability to use the software.
 */

/**
 * This function performs a read or a take operation on a remote DataReader
 * given the URL that identifies it. Upon success, if new data becoms available,
 * the function calls onDataAvailableFnc that is given as a parameter.
 * The function performs Long Polling read operation using a maxWait timeout
 * of 30 seconds, which is the time Web Integration Service's internal waitset
 * will block until there are samples with sampleStateMask = "NOT_READ."
 * @param datareaderUrl URL identifying the remote DataReader.
 * @param onDataAvailableFnc Function that will handle data if it becomes
 * available.
 * @param take Boolean parameter indicating whether data that is read should
 * be removed from the remote DataReader's cache.
 */
function readOrTake(datareaderUrl, onDataAvailableFnc, take)
{
    // Timeout in seconds
    var maxWait = 30;

    // Do HTTP request, which calls read_or_take again upon completion
    $.ajax({
        url: datareaderUrl,
        success: function(data, textStatus, xhr) {
            if (xhr.status == 200 && data.length > 0) {
                // Call  onDataAvailableFnc only if data != []
                onDataAvailableFnc(data)
            }
        },
        error: function(xhr, textStatus, errorThrown) {
            console.log(errorThrown);
        },
        complete: function(xhr) {
            /*
             * If Web Integration Service stops or the connection is dropped,
             * the client appplication will spin because $.ajax will complete
             * the request right away without waiting for "timeout" ms.
             * We recommend checking for this kind of errors and introducing a
             * re-try mechanism to avoidspinning while the client reconnects
             * to the server.
             */
            readOrTake(datareaderUrl, onDataAvailableFnc)
        },
        dataType: "json",
        data: {
            sampleFormat: "json",
            removeFromReaderCache: (take ? "true" : "false"),
            maxWait: maxWait,
            sampleStateMask: "NOT_READ"
        },
        timeout: (maxWait * 1000 * 2)})
}

/**
 * This function performs a read operation that calls onDataAvailableFnc
 * when new samples become available. The read operation does not remove
 * samples from the DataReader's cache.
 * @param datareaderUrl URL identifying the remote DataReader.
 * @param onDataAvailableFnc Function that will handle data if it becomes
 * available.
 */
function read(datareaderUrl, onDataAvailableFnc)
{
    readOrTake(datareaderUrl, onDataAvailableFnc, false)
}

/**
 * This function performs a take operation that calls onDataAvailableFnc
 * when new samples become available. The take operation removes samples from
 * the DataReader's cache.
 * @param datareaderUrl URL identifying the remote DataReader.
 * @param onDataAvailableFnc Function that will handle data if it becomes
 * available.
 */
function take(datareaderUrl, onDataAvailableFnc)
{
    readOrTake(datareaderUrl, onDataAvailableFnc, true)
}
