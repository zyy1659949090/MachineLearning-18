/** 
 @cond
 ############################################################################
 # LGPL License                                                             #
 #                                                                          #
 # This file is part of the Machine Learning Framework.                     #
 # Copyright (c) 2010-2012, Philipp Kraus, <philipp.kraus@flashpixx.de>     #
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

#ifndef __MACHINELEARNING_EXCEPTION_HPP
#define __MACHINELEARNING_EXCEPTION_HPP

#include <string>
#include <stdexcept>

#include "../tools/typeinfo.h"
#include "../tools/logger.hpp"


namespace machinelearning { 
    
    /** namespace for all exceptions of the framework
     * @todo adding more exceptions for a better error handling
     * $LastChangedDate$
     **/
    namespace exception {
    
        /** exception class for throwing on not implementated method 
         * $LastChangedDate$
         **/
        class classmethod : public std::logic_error
        { 
            public : 
            
                /** creates the exception with a message
                 * @param p_msg error message
                 **/
                explicit classmethod( const std::string& p_msg) :
                    std::logic_error( p_msg )
                {}
            
            
                /** creates the exception with a message and a classname
                 * @param p_msg error message
                 * @param p_ptr pointer to a class object
                 **/
                template <typename T> explicit classmethod( const std::string& p_msg, const T* p_ptr ) :
                    std::logic_error( p_msg + ( !tools::typeinfo::getClassName(p_ptr).empty() ? " ["+tools::typeinfo::getClassName(p_ptr)+"]" : "") )
                {}  
            
            
                /** creates the exception with a message and a classname
                 * @param p_msg error message
                 * @param p_obj reference to an class object
                 **/
                template <typename T> explicit classmethod( const std::string& p_msg, const T& p_obj ) :
                    std::logic_error( p_msg + ( !tools::typeinfo::getClassName(p_obj).empty() ? " ["+tools::typeinfo::getClassName(p_obj)+"]" : "") )
                {}  
        };
        
        
        
        /** exception class for throwing on runtime errors
         * $LastChangedDate$
         **/
        class runtime : public std::runtime_error
        { 
            public : 
                
                /** creates the exception with a message
                 * @param p_msg error message
                 **/
                explicit runtime( const std::string& p_msg ) :
                    std::runtime_error( p_msg )
                {
                    #ifdef MACHINELEARNING_LOGGER
                    if (tools::logger::exists())
                        tools::logger::getInstance()->write( tools::logger::exception, "runtime exception is thrown with message: " + p_msg);
                    #endif
                }  
            
            
                /** creates the exception with a message and a class name
                 * @param p_msg error message
                 * @param p_ptr pointer to an class object
                 **/
                template <typename T> explicit runtime( const std::string& p_msg, const T* p_ptr ) :
                    std::runtime_error( p_msg + ( !tools::typeinfo::getClassName(p_ptr).empty() ? " ["+tools::typeinfo::getClassName(p_ptr)+"]" : "") )
                {}  
            
            
                /** creates the exception with a message and a class name
                 * @param p_msg error message
                 * @param p_obj reference to a classobject
                 **/
                template <typename T> explicit runtime( const std::string& p_msg, const T& p_obj ) :
                    std::runtime_error( p_msg + ( !tools::typeinfo::getClassName(p_obj).empty() ? " ["+tools::typeinfo::getClassName(p_obj)+"]" : "") )
                {}  
        };
             
        
    }
}
#endif