/** 
 @cond
 ############################################################################
 # LGPL License                                                             #
 #                                                                          #
 # This file is part of the Machine Learning Framework.                     #
 # Copyright (c) 2010, Philipp Kraus, <philipp.kraus@flashpixx.de>          #
 # This program is free software: you can redistribute it and/or modify     #
 # it under the terms of the GNU Lesser General Public License as           #
 # published by the Free Software Foundation, either version 3 of the       #
 # License, or (at your option) any later version.                          #
 #                                                                          #
 # This program is distributed in the hope that it will be useful,          #
 # but WITHOUT ANY WARRANTY; without even the implied warranty of           #
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            #
 # GNU Lesser General Public License for more details.                      #
 #                                                                          #
 # You should have received a copy of the GNU Lesser General Public License #
 # along with this program. If not, see <http://www.gnu.org/licenses/>.     #
 ############################################################################
 @endcond
 **/

#ifndef __MACHINELEARNING_SWIG_JAVA_HPP
#define __MACHINELEARNING_SWIG_JAVA__HPP

#include <jni.h>

#include "../../../machinelearning.h"


namespace machinelearning { namespace swig {
    
    namespace ublas = boost::numeric::ublas;
    
    
    /** class for generate fragment code, that is used to convert
     * numericial structurs of Java to C++ UBlas (both ways), it is called by
     * the SWIG fragment calls
     * $LastChangedDate$
     * @todo add copy process with OpenMP
     **/
    class convert {
        
        public :
        
            /** row type **/
            enum rowtype
            {
                row     = 0,
                column  = 1
            };
            
            
            static ublas::matrix<double> getDoubleMatrixFrom2DArray( JNIEnv*, const jobjectArray& );
            static ublas::matrix<float> getFloatMatrixFrom2DArray( JNIEnv*, const jobjectArray& );
            static jobjectArray getArrayFromMatrix( JNIEnv*, const ublas::matrix<double>&, const rowtype& = row );
            static jobjectArray getArrayFromMatrix( JNIEnv*, const ublas::matrix<float>&, const rowtype& = row );
            static jobjectArray getArrayFromVector( JNIEnv*, const ublas::vector<double>& );
            static jobjectArray getArrayFromVector( JNIEnv*, const ublas::vector<float>& );
            static std::string getString( JNIEnv*, const jstring );
            static std::vector<std::string> getStringVectorFromArray( JNIEnv*, const jobjectArray& );
        
        private :
        
            static jmethodID getMethodID(JNIEnv*, const jobject&, const char*, const char*);
            static jmethodID getMethodID(JNIEnv*, const char*, const char*, const char*);
            static void getCtor(JNIEnv*, const char*, const char*, jclass&, jmethodID&);
        
    };
    
           
    /** returns the method id of an object
     * @param p_env JNI environment
     * @param p_object JNI object
     * @param p_name method name
     * @param p_signatur method signature
     * @return method id
     * @todo check jmethodID use for memory leak
     **/
    inline jmethodID convert::getMethodID(JNIEnv* p_env, const jobject& p_object, const char* p_name, const char* p_signatur)
    {
        jclass l_class = p_env->GetObjectClass( p_object );
        if (!l_class) {
            SWIG_JavaThrowException(p_env, SWIG_JavaRuntimeException, "can not find associated java class");
            return 0;
        }
        
        jmethodID l_id = p_env->GetMethodID(l_class, p_name, p_signatur);
        if (!l_id) {
            SWIG_JavaThrowException(p_env, SWIG_JavaRuntimeException, "can not find method with signature" );
            return 0;
        }
        
        p_env->DeleteLocalRef(l_class);
        return l_id;
    }
    
    
    /** returns the method id of a class
     * @param p_env JNI environment
     * @param p_classname class name
     * @param p_methodname method name
     * @param p_signatur method signature
     * @return method id
     * @todo check jmethodID use for memory leak
     **/
    inline jmethodID convert::getMethodID(JNIEnv* p_env, const char* p_classname, const char* p_methodname, const char* p_signatur)
    {
        jclass l_class = p_env->FindClass(p_classname);
        if (!l_class) {
            SWIG_JavaThrowException(p_env, SWIG_JavaRuntimeException, "can not find associated java class" );
            return 0;
        }
        
        jmethodID l_id = p_env->GetMethodID(l_class, p_methodname, p_signatur);
        if (!l_id) {
            SWIG_JavaThrowException(p_env, SWIG_JavaRuntimeException, "can not find method with signature" );
            return 0;
        }
        
        return l_id;
    }
    
    
    /** create resources to the constructor call
     * @param p_env JNI environment
     * @param p_name name of the class
     * @param p_signatur signatur for the constructor
     * @param p_classid reference to the class id
     * @param p_ctorid reference to the ctor method id
     **/
    inline void convert::getCtor(JNIEnv* p_env, const char* p_name, const char* p_signatur, jclass& p_classid, jmethodID& p_ctorid)
    {
        p_ctorid  = 0;
        p_classid = p_env->FindClass(p_name);
        if (!p_classid)
            SWIG_JavaThrowException(p_env, SWIG_JavaRuntimeException, "can not find associated java class" );
        else {
            p_ctorid = p_env->GetMethodID(p_classid, "<init>", p_signatur);
            if (!p_ctorid)
                SWIG_JavaThrowException(p_env, SWIG_JavaRuntimeException, "can not find constructor call" );
        }
    }
    
    
    /** creates a ublas double matrix from a java 2D array
     * @param p_env JNI environment
     * @param p_data java array
     * @return ublas matrix if matrix have zero columns and/or rows the array can not be read
     **/
    inline ublas::matrix<double> convert::getDoubleMatrixFrom2DArray( JNIEnv* p_env, const jobjectArray& p_data )
    {
        ublas::matrix<double> l_data(0,0);
        
        // convert the java array to a ublas matrix (first read the row dimension and than read the first array element, cast it to jobjectArray and get the length)
        const std::size_t l_rows = p_env->GetArrayLength(p_data);
        if (l_rows == 0)
            return l_data;
        
        const std::size_t l_cols = p_env->GetArrayLength( (jobjectArray)p_env->GetObjectArrayElement(p_data, 0) );
        if (l_cols == 0)
            return l_data;
        
        // each element in the array is a "java.lang.Double" value, for reading the value the method "double doubleValue()" must be called, so get the ID
        const jmethodID l_valueof = getMethodID(p_env, "java/lang/Double", "doubleValue", "()D"); 
        
        // read array data
        l_data = ublas::matrix<double>(l_rows, l_cols);
        for(std::size_t i=0; i < l_rows; ++i) {
            jobjectArray l_coldata = (jobjectArray)p_env->GetObjectArrayElement(p_data, i);
            
            for(std::size_t j=0; j < std::min(l_cols, static_cast<std::size_t>(p_env->GetArrayLength(l_coldata))); ++j) {
                l_data(i,j) = p_env->CallDoubleMethod( p_env->GetObjectArrayElement(l_coldata, j), l_valueof );
                if (tools::function::isNumericalZero(l_data(i,j))) 
                    l_data(i,j) = static_cast<double>(0);
            }
        }
        
        return l_data;
    }
    
    
    /** creates a ublas float matrix from a java 2D array
     * @param p_env JNI environment
     * @param p_data java array
     * @return ublas matrix if matrix have zero columns and/or rows the array can not be read
     **/
    inline ublas::matrix<float> convert::getFloatMatrixFrom2DArray( JNIEnv* p_env, const jobjectArray& p_data )
    {
        ublas::matrix<float> l_data(0,0);
        
        // convert the java array to a ublas matrix (first read the row dimension and than read the first array element, cast it to jobjectArray and get the length)
        const std::size_t l_rows = p_env->GetArrayLength(p_data);
        if (l_rows == 0)
            return l_data;
        
        const std::size_t l_cols = p_env->GetArrayLength( (jobjectArray)p_env->GetObjectArrayElement(p_data, 0) );
        if (l_cols == 0)
            return l_data;
        
        // each element in the array is a "java.lang.Float" value, for reading the value the method "float floatValue()" must be called, so get the ID
        const jmethodID l_valueof = getMethodID(p_env, "java/lang/Float", "floatValue", "()F"); 
        
        // read array data
        l_data = ublas::matrix<float>(l_rows, l_cols);
        for(std::size_t i=0; i < l_rows; ++i) {
            jobjectArray l_coldata = (jobjectArray)p_env->GetObjectArrayElement(p_data, i);
            
            for(std::size_t j=0; j < std::min(l_cols, static_cast<std::size_t>(p_env->GetArrayLength(l_coldata))); ++j) {
                l_data(i,j) = p_env->CallFloatMethod( p_env->GetObjectArrayElement(l_coldata, j), l_valueof );
                if (tools::function::isNumericalZero(l_data(i,j))) 
                    l_data(i,j) = static_cast<float>(0);
            }
        }
        
        return l_data;
    }
    
    
    /** creates a 2D java array of an ublas double matrix
     * @param p_env JNI environment
     * @param p_data input data matrix
     * @param p_rowtype row type
     * @return java array / or a null object if the matrix is empty
     * @todo add numerical zero change
     **/
    inline jobjectArray convert::getArrayFromMatrix( JNIEnv* p_env, const ublas::matrix<double>& p_data, const rowtype& p_rowtype )
    {
        if ( (p_data.size1() == 0) || (p_data.size2() == 0) )
            return (jobjectArray)p_env->NewGlobalRef(NULL);
        
        jclass l_elementclass   = NULL;
        jmethodID l_elementctor = NULL;
        getCtor(p_env, "java/lang/Double", "(D)V", l_elementclass, l_elementctor);
        
        switch (p_rowtype) {
                
            case row: {
                // create the row array
                jobjectArray l_row = p_env->NewObjectArray( static_cast<jint>(p_data.size1()), p_env->FindClass("[Ljava/lang/Double;"), NULL );
                for(std::size_t i=0; i < p_data.size1(); ++i) {
                    
                    // create column array and fill data into the double java object
                    jobjectArray l_col = p_env->NewObjectArray( static_cast<jint>(p_data.size2()), l_elementclass, NULL );
                    for(std::size_t j=0; j < p_data.size2(); ++j)
                        p_env->SetObjectArrayElement(l_col, j, p_env->NewObject(l_elementclass, l_elementctor, tools::function::isNumericalZero(p_data(i,j)) ? static_cast<double>(0) : p_data(i,j) ) );
                    
                    p_env->SetObjectArrayElement(l_row, i, l_col);
                }
                
                return l_row;
            }
                
                
            case column: {
                // create the row array
                jobjectArray l_row = p_env->NewObjectArray( static_cast<jint>(p_data.size2()), p_env->FindClass("[Ljava/lang/Double;"), NULL );
                for(std::size_t i=0; i < p_data.size2(); ++i) {
                    
                    // create column array and fill data into the double java object
                    jobjectArray l_col = p_env->NewObjectArray( static_cast<jint>(p_data.size1()), l_elementclass, NULL );
                    for(std::size_t j=0; j < p_data.size1(); ++j)
                        p_env->SetObjectArrayElement(l_col, j, p_env->NewObject(l_elementclass, l_elementctor,tools::function::isNumericalZero(p_data(i,j)) ? static_cast<double>(0) : p_data(i,j)) );
                    
                    p_env->SetObjectArrayElement(l_row, i, l_col);
                }
                
                return l_row;
            }
        }
        
        return (jobjectArray)p_env->NewGlobalRef(NULL);
    }
    
    
    /** creates a 2D java array of an ublas float matrix
     * @param p_env JNI environment
     * @param p_data input data matrix
     * @param p_rowtype row type
     * @return java array / or a null object if the matrix is empty
     * @todo add numerical zero change
     **/
    inline jobjectArray convert::getArrayFromMatrix( JNIEnv* p_env, const ublas::matrix<float>& p_data, const rowtype& p_rowtype )
    {
        if ( (p_data.size1() == 0) || (p_data.size2() == 0) )
            return (jobjectArray)p_env->NewGlobalRef(NULL);
        
        jclass l_elementclass   = NULL;
        jmethodID l_elementctor = NULL;
        getCtor(p_env, "java/lang/Float", "(F)V", l_elementclass, l_elementctor);
        
        switch (p_rowtype) {
                
            case row: {
                // create the row array
                jobjectArray l_row = p_env->NewObjectArray( static_cast<jint>(p_data.size1()), p_env->FindClass("[Ljava/lang/Float;"), NULL );
                for(std::size_t i=0; i < p_data.size1(); ++i) {
                    
                    // create column array and fill data into the double java object
                    jobjectArray l_col = p_env->NewObjectArray( static_cast<jint>(p_data.size2()), l_elementclass, NULL );
                    for(std::size_t j=0; j < p_data.size2(); ++j)
                        p_env->SetObjectArrayElement(l_col, j, p_env->NewObject(l_elementclass, l_elementctor, tools::function::isNumericalZero(p_data(i,j)) ? static_cast<float>(0) : p_data(i,j)) );
                    
                    p_env->SetObjectArrayElement(l_row, i, l_col);
                }
                
                return l_row;
            }
                
                
            case column : {
                // create the column array
                jobjectArray l_row = p_env->NewObjectArray( static_cast<jint>(p_data.size2()), p_env->FindClass("[Ljava/lang/Float;"), NULL );
                for(std::size_t i=0; i < p_data.size2(); ++i) {
                    
                    // create row array and fill data into the double java object
                    jobjectArray l_col = p_env->NewObjectArray( static_cast<jint>(p_data.size1()), l_elementclass, NULL );
                    for(std::size_t j=0; j < p_data.size1(); ++j)
                        p_env->SetObjectArrayElement(l_col, j, p_env->NewObject(l_elementclass, l_elementctor, tools::function::isNumericalZero(p_data(i,j)) ? static_cast<float>(0) : p_data(i,j)) );
                    
                    p_env->SetObjectArrayElement(l_row, i, l_col);
                }
                
                return l_row;
            }
        }
        
        return (jobjectArray)p_env->NewGlobalRef(NULL);
    }
    
    
    /** converts a ublas::vector to a java array
     * @param p_env JNI environment
     * @param p_data vector
     * @return java array
     * @todo add numerical zero change
     **/
    inline jobjectArray convert::getArrayFromVector( JNIEnv* p_env, const ublas::vector<double>& p_data )
    {
        if (p_data.size() == 0)
            return (jobjectArray)p_env->NewGlobalRef(NULL);
        
        jclass l_elementclass   = NULL;
        jmethodID l_elementctor = NULL;
        getCtor(p_env, "java/lang/Double", "(D)V", l_elementclass, l_elementctor);
        
        jobjectArray l_vec = p_env->NewObjectArray( static_cast<jint>(p_data.size()), l_elementclass, NULL );
        for(std::size_t i=0; i < p_data.size(); ++i)
            p_env->SetObjectArrayElement(l_vec, i, p_env->NewObject(l_elementclass, l_elementctor, tools::function::isNumericalZero(p_data(i)) ? static_cast<double>(0) : p_data(i)) );
        
        return l_vec;
    }
    
    
    /** converts a ublas::vector to a java array
     * @param p_env JNI environment
     * @param p_data vector
     * @return java array
     * @todo add numerical zero change
     **/
    inline jobjectArray convert::getArrayFromVector( JNIEnv* p_env, const ublas::vector<float>& p_data )
    {
        if (p_data.size() == 0)
            return (jobjectArray)p_env->NewGlobalRef(NULL);
        
        jclass l_elementclass   = NULL;
        jmethodID l_elementctor = NULL;
        getCtor(p_env, "java/lang/Float", "(F)V", l_elementclass, l_elementctor);
        
        jobjectArray l_vec = p_env->NewObjectArray( static_cast<jint>(p_data.size()), l_elementclass, NULL );
        for(std::size_t i=0; i < p_data.size(); ++i)
            p_env->SetObjectArrayElement(l_vec, i, p_env->NewObject(l_elementclass, l_elementctor, tools::function::isNumericalZero(p_data(i)) ? static_cast<float>(0) : p_data(i)) );
        
        return l_vec;
    }
    
    
    /** converts a jstring into a std::string
     * @param p_env JNI environment
     * @param p_data jstring
     ** @return std::string
     **/
    inline std::string convert::getString( JNIEnv* p_env, const jstring p_data ) {
        return std::string(p_env->GetStringUTFChars(p_data, NULL));
    }
    
    inline std::vector<std::string> convert::getStringVectorFromArray( JNIEnv* p_env, const jobjectArray& p_data ) {
        std::vector<std::string> l_data;
        
        // convert the java array to a ublas matrix (first read the row dimension and than read the first array element, cast it to jobjectArray and get the length)
        const std::size_t l_len = p_env->GetArrayLength(p_data);
        if (l_len == 0)
            return l_data;
                
        // read array data
        for(std::size_t i=0; i < l_len; ++i)
            l_data.push_back( getString(p_env, (jstring)p_env->GetObjectArrayElement(p_data, i)) );
        
        return l_data;
    }
    
}}
#endif
