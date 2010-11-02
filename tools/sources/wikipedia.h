/** 
 @cond
 #########################################################################
 # GPL License                                                           #
 #                                                                       #
 # This file is part of the Machine Learning Framework.                  #
 # Copyright (c) 2010, Philipp Kraus, <philipp.kraus@flashpixx.de>       #
 # This program is free software: you can redistribute it and/or modify  #
 # it under the terms of the GNU General Public License as published by  #
 # the Free Software Foundation, either version 3 of the License, or     #
 # (at your option) any later version.                                   #
 #                                                                       #
 # This program is distributed in the hope that it will be useful,       #
 # but WITHOUT ANY WARRANTY; without even the implied warranty of        #
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
 # GNU General Public License for more details.                          #
 #                                                                       #
 # You should have received a copy of the GNU General Public License     #
 # along with this program.  If not, see <http://www.gnu.org/licenses/>. #
 #########################################################################
 @endcond
 **/

#ifdef SOURCES

#ifndef MACHINELEARNING_TOOLS_SOURCES_WIKIPEDIA_H
#define MACHINELEARNING_TOOLS_SOURCES_WIKIPEDIA_H

#include <boost/asio.hpp>
#include <rapidxml/rapidxml.hpp>

#include "../../exception/exception.h"
#include "../language/language.h"


namespace machinelearning { namespace tools { namespace sources {

    namespace bip  = boost::asio::ip;
    
    
    /** class for reading Wikipedia article.
     * The data will received over a HTTP socket for each call and uses RapidXML library for parsing the XML structur
     * @see http://rapidxml.sourceforge.net/
     **/
    class wikipedia {
        
        public :
        
            enum language {
                en_EN   = 0,
                de_DE   = 1
            };
        
            wikipedia( const language& = de_DE );
            void getArticle( const std::string&, const language& = de_DE );
            void getRandomArticle( const language& = de_DE );
        
        
        private :
        
            /** definition for URL **/
            struct url {
                std::string host;
                std::string path;
            };
        
            /** definition for language **/
            struct wikiproperties {
                language lang;
                url exporturl;
                url randomurl;
                std::string acronymref;
                std::string acronym;
                std::string category;
                std::string titleadd;
            };
        
        
            /** default wikipedia properties **/
            const wikiproperties m_defaultproperties;
            /** io service objekt for resolving the server name**/
            boost::asio::io_service m_io;
            /** socket objekt for send / receive the data **/
            bip::tcp::socket m_socket; 
        
            wikiproperties getProperties( const language& ) const;
            std::string getRequest( const std::string&, const std::string& );

        
    };
    
    
    /** constructor
     * @apram p_lang optional language parameter (default de_DE)
     **/
    inline wikipedia::wikipedia( const language& p_lang ) :
        m_defaultproperties( getProperties(p_lang) ),
        m_io(),
        m_socket(m_io)
    {}
    

    /** creates the properties of a language 
     * @return struct with Wikipedia properties for extraction article information
     **/
    inline wikipedia::wikiproperties wikipedia::getProperties( const language& p_lang ) const
    {
        wikiproperties l_prop;
        
        l_prop.lang = p_lang;
        switch (p_lang) {
            
           case en_EN :
                l_prop.exporturl.host   = "en.wikipedia.org";
                l_prop.exporturl.path   = "/wiki/Special:Export/";
                l_prop.randomurl.host   = "en.wikipedia.org";
                l_prop.randomurl.path   = "/wiki/Special:Random";
                l_prop.acronymref       = "can refer to";
                l_prop.acronym          = "Acronyms";
                l_prop.category         = "Category";
                l_prop.titleadd         = "- Wikipedia, the free encyclopedia";
                return l_prop;
                
            case de_DE :
                l_prop.exporturl.host   = "de.wikipedia.org";
                l_prop.exporturl.path   = "/wiki/Spezial:Exportieren/";
                l_prop.randomurl.host   = "de.wikipedia.org";
                l_prop.randomurl.path   = "/wiki/Spezial:Zuf%C3%A4llige_Seite";
                l_prop.acronymref       = "steht für";
                l_prop.acronym          = "Abkürzung";
                l_prop.category         = "Kategorie";
                l_prop.titleadd         = "Wikipedia";
                return l_prop;

        }
        
        throw exception::parameter(_("language is not kwon"));
    }
    
    
    /** reads an article
     * @param p_search keyword for searching
     * @param p_lang optional language
     **/
    void wikipedia::getArticle( const std::string& p_search, const language& p_lang )
    {
        wikiproperties l_prop = m_defaultproperties;
        if (l_prop.lang != p_lang)
            l_prop = getProperties( p_lang );
        
        getRequest( l_prop.exporturl.host, l_prop.exporturl.path + p_search );
    }
    
    
    /** reads an random article
     * @param p_lang optional language
     **/
    void wikipedia::getRandomArticle( const language& p_lang )
    {
        wikiproperties l_prop = m_defaultproperties;
        if (l_prop.lang != p_lang)
            l_prop = getProperties( p_lang );
    }
    
    
    std::string wikipedia::getRequest( const std::string& p_server, const std::string& p_path )
    {
        // create resolver for server
        bip::tcp::resolver l_resolver(m_io);
        bip::tcp::resolver::query l_query(p_server, "http");
        
        // try to connect the server
        bip::tcp::resolver::iterator l_endpoint = l_resolver.resolve( l_query );
        bip::tcp::resolver::iterator l_endpointend;
        boost::system::error_code l_error       = boost::asio::error::host_not_found;
        
        while (l_error && l_endpoint != l_endpointend) {
            m_socket.close();
            m_socket.connect(*l_endpoint++, l_error);
        }
        
        if (l_error)
            throw exception::parameter(_("cannot connect to wikipedia server"));
        
        // create HTTP request
        boost::asio::streambuf l_request;
        std::ostream l_request_stream(&l_request);
        l_request_stream << "GET " << p_path << " HTTP/1.0\r\n";
        l_request_stream << "Host: " << p_server << "\r\n";
        l_request_stream << "Accept: */*\r\n";
        l_request_stream << "Connection: close\r\n\r\n";
        
        
        std::string l_xml;
        return l_xml;
        
    }
    
    
};};};


#endif
#endif