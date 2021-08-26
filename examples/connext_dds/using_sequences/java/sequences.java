

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

public class sequences   implements Copyable, Serializable{

    public String id= (String)""; /* maximum length = (100) */
    public short count = (short)0;
    public ShortSeq data =  new ShortSeq((MAX_SEQUENCE_LEN.VALUE));

    public sequences() {

    }
    public sequences (sequences other) {

        this();
        copy_from(other);
    }

    public static java.lang.Object create() {

        sequences self;
        self = new  sequences();
        self.clear();
        return self;

    }

    public void clear() {

        id = (String)"";
        count = (short)0;
        if (data != null) {
            data.clear();
        }
    }

    @Override
    public boolean equals(java.lang.Object o) {

        if (o == null) {
            return false;
        }        

        if(getClass() != o.getClass()) {
            return false;
        }

        sequences otherObj = (sequences)o;

        if(!this.id.equals(otherObj.id)) {
            return false;
        }
        if(this.count != otherObj.count) {
            return false;
        }
        if(!this.data.equals(otherObj.data)) {
            return false;
        }

        return true;
    }

    @Override
    public int hashCode() {
        int __result = 0;
        __result += id.hashCode(); 
        __result += (int)count;
        __result += data.hashCode(); 
        return __result;
    }

    /**
    * This is the implementation of the <code>Copyable</code> interface.
    * This method will perform a deep copy of <code>src</code>
    * This method could be placed into <code>sequencesTypeSupport</code>
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

        sequences typedSrc = (sequences) src;
        sequences typedDst = this;

        typedDst.id = typedSrc.id;
        typedDst.count = typedSrc.count;
        typedDst.data.copy_from(typedSrc.data);

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
        strBuffer.append("id: ").append(this.id).append("\n");  
        CdrHelper.printIndent(strBuffer, indent+1);        
        strBuffer.append("count: ").append(this.count).append("\n");  
        CdrHelper.printIndent(strBuffer, indent+1);
        strBuffer.append("data: ");
        for(int i__ = 0; i__ < this.data.size(); ++i__) {
            if (i__!=0) strBuffer.append(", ");
            strBuffer.append(this.data.get(i__));
        }
        strBuffer.append("\n"); 

        return strBuffer.toString();
    }

}
