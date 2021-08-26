

/*
WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

This file was generated from .idl 
using RTI Code Generator (rtiddsgen) version 3.1.0.
The rtiddsgen tool is part of the RTI Connext DDS distribution.
For more information, type 'rtiddsgen -help' at a command shell
or consult the Code Generator User's Manual.
*/

import com.rti.dds.infrastructure.*;
import com.rti.dds.infrastructure.Copyable;
import java.io.Serializable;
import com.rti.dds.cdr.CdrHelper;

public class numbers   implements Copyable, Serializable{

    public int number = (int)0;
    public float halfNumber = (float)0;

    public numbers() {

    }
    public numbers (numbers other) {

        this();
        copy_from(other);
    }

    public static java.lang.Object create() {

        numbers self;
        self = new  numbers();
        self.clear();
        return self;

    }

    public void clear() {

        number = (int)0;
        halfNumber = (float)0;
    }

    @Override
    public boolean equals(java.lang.Object o) {

        if (o == null) {
            return false;
        }        

        if(getClass() != o.getClass()) {
            return false;
        }

        numbers otherObj = (numbers)o;

        if(this.number != otherObj.number) {
            return false;
        }
        if(this.halfNumber != otherObj.halfNumber) {
            return false;
        }

        return true;
    }

    @Override
    public int hashCode() {
        int __result = 0;
        __result += (int)number;
        __result += (int)halfNumber;
        return __result;
    }

    /**
    * This is the implementation of the <code>Copyable</code> interface.
    * This method will perform a deep copy of <code>src</code>
    * This method could be placed into <code>numbersTypeSupport</code>
    * rather than here by using the <code>-noCopyable</code> option
    * to rtiddsgen.
    * 
    * @param src The Object which contains the data to be copied.
    * @return Returns <code>this</code>.
    * @exception NullPointerException If <code>src</code> is null.
    * @exception ClassCastException If <code>src</code> is not the 
    * same type as <code>this</code>.
    * @see com.rti.dds.infrastructure.Copyable#copy_from(java.lang.Object)
    */
    public java.lang.Object copy_from(java.lang.Object src) {

        numbers typedSrc = (numbers) src;
        numbers typedDst = this;

        typedDst.number = typedSrc.number;
        typedDst.halfNumber = typedSrc.halfNumber;

        return this;
    }

    @Override
    public java.lang.String toString(){
        return toString("", 0);
    }

    public java.lang.String toString(java.lang.String desc, int indent) {
        java.lang.StringBuffer strBuffer = new java.lang.StringBuffer();

        if (desc != null) {
            CdrHelper.printIndent(strBuffer, indent);
            strBuffer.append(desc).append(":\n");
        }

        CdrHelper.printIndent(strBuffer, indent+1);        
        strBuffer.append("number: ").append(this.number).append("\n");  
        CdrHelper.printIndent(strBuffer, indent+1);        
        strBuffer.append("halfNumber: ").append(this.halfNumber).append("\n");  

        return strBuffer.toString();
    }

}
