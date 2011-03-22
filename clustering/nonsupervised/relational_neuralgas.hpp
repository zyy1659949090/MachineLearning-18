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


#ifndef MACHINELEARNING_CLUSTERING_NONSUPERVISED_RNG_HPP
#define MACHINELEARNING_CLUSTERING_NONSUPERVISED_RNG_HPP

#include <numeric>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

#ifdef MACHINELEARNING_MPI
#include <boost/mpi.hpp>
#endif

#include "../clustering.hpp"
#include "../../exception/exception.h"
#include "../../tools/tools.h"
#include "../../neighborhood/neighborhood.h"



namespace machinelearning { namespace clustering { namespace nonsupervised {
    
    namespace ublas = boost::numeric::ublas;
    #ifdef MACHINELEARNING_MPI
    namespace mpi   = boost::mpi;
    #endif
    
    /** class for calculate (batch) relational neural gas
     * $LastChangedDate$
     * @note The MPI methods do not check the correct ranges / dimension of the prototype
     * data, so it is the task of the developer to use the correct ranges. Also the MPI
     * methods must be called in the correct order, so the MPI calls must be run
     * on each process.
     * @todo thinking about relation calculating transform to a own distance class
     **/
    template<typename T> class relational_neuralgas : public clustering<T>, public patch<T> {
        BOOST_STATIC_ASSERT( !boost::is_integral<T>::value );
        
        public:
        
            relational_neuralgas( const std::size_t&, const std::size_t& );
            //relational_neuralgas( const std::size_t&, const std::size_t&, const neighborhood::kapproximation<T>& );
            void train( const ublas::matrix<T>&, const std::size_t& );
            void train( const ublas::matrix<T>&, const std::size_t&, const T& );
            ublas::matrix<T> getPrototypes( void ) const;
            void setLogging( const bool& );
            std::vector< ublas::matrix<T> > getLoggedPrototypes( void ) const;
            bool getLogging( void ) const;
            std::size_t getPrototypeSize( void ) const;
            std::size_t getPrototypeCount( void ) const;
            std::vector<T> getLoggedQuantizationError( void ) const;
            ublas::indirect_array<> use( const ublas::matrix<T>& ) const;
        
            #ifdef MACHINELEARNING_MPI
            void train( const mpi::communicator&, const ublas::matrix<T>&, const std::size_t& );
            void train( const mpi::communicator&, const ublas::matrix<T>&, const std::size_t&, const T& );
            //ublas::matrix<T> getPrototypes( const mpi::communicator& ) const;
            //std::vector< ublas::matrix<T> > getLoggedPrototypes( const mpi::communicator& ) const;
            //std::vector<T> getLoggedQuantizationError( const mpi::communicator& ) const;
            //ublas::indirect_array<> use( const mpi::communicator&, const ublas::matrix<T>& ) const;
            //void use( const mpi::communicator& ) const;
            #endif
        
        
        private:
        
            /** prototypes **/
            ublas::matrix<T> m_prototypes;
            /** k-approximation object
             * @todo swap pointer to reference and initialize it with default object
             **/
            const neighborhood::kapproximation<T>* m_kapprox;
            /** bool for logging prototypes **/
            bool m_logging;
            /** std::vector for prototypes for each iteration **/
            std::vector< ublas::matrix<T> > m_logprototypes;
            /** std::vector for quantisation error in each iteration **/
            std::vector<T> m_quantizationerror;
            /** bool for check initialized patch **/
            bool m_firstpatch;
        
            T calculateQuantizationError( const ublas::matrix<T>& ) const;
        
            #ifdef MACHINELEARNING_MPI
            std::size_t getNumberPrototypes( const mpi::communicator& ) const;
            #endif
        
    };

    
    /** contructor for initialization the neural gas
     * @param p_prototypes number of prototypes
     * @param p_prototypesize size of each prototype (data dimension)
     **/
    template<typename T> inline relational_neuralgas<T>::relational_neuralgas( const std::size_t& p_prototypes, const std::size_t& p_prototypesize ) :
        m_prototypes( tools::matrix::random<T>(p_prototypes, p_prototypesize) ),
        m_kapprox( NULL ),
        m_logging( false ),
        m_logprototypes( std::vector< ublas::matrix<T> >() ),
        m_quantizationerror( std::vector<T>() ),
        m_firstpatch(true)
    {
        if (p_prototypesize == 0)
            throw exception::runtime(_("prototype size must be greater than zero"));
 
        // normalize the prototypes
        for(std::size_t i=0; i <  m_prototypes.size1(); ++i) {
            const T l_sum = ublas::sum( ublas::row( m_prototypes, i) );
            
            if (!tools::function::isNumericalZero(l_sum))
                ublas::row( m_prototypes, i) /= l_sum;
        }
    }
    
    
    
    /** contructor for initialization the neural gas
     * @param p_prototypes number of prototypes
     * @param p_prototypesize size of each prototype (data dimension)
     * @param p_kapprox k-approximation object
     **
    template<typename T> inline relational_neuralgas<T>::relational_neuralgas( const std::size_t& p_prototypes, const std::size_t& p_prototypesize, const neighborhood::kapproximation<T>& p_kapprox ) :
        m_prototypes( tools::matrix::random<T>(p_prototypes, p_prototypesize) ),
        m_kapprox( &p_kapprox ),
        m_logging( false ),
        m_logprototypes( std::vector< ublas::matrix<T> >() ),
        m_quantizationerror( std::vector<T>() ),
        m_firstpatch(true)
    {
        if (p_prototypesize == 0)
            throw exception::runtime(_("prototype size must be greater than zero"));
        
        // normalize the prototypes
        for(std::size_t i=0; i <  m_prototypes.size1(); ++i) {
            const T l_sum = ublas::sum( ublas::row( m_prototypes, i) );
            
            if (!tools::function::isNumericalZero(l_sum))
                ublas::row( m_prototypes, i) /= l_sum;
        }
    }*/
    

    
    /** returns the prototype matrix
     * @return matrix (rows = number of prototypes)
     **/
    template<typename T> inline ublas::matrix<T> relational_neuralgas<T>::getPrototypes( void ) const
    {
        return m_prototypes;
    }
    
    
    
    /** enabled logging for training
     * @param p bool
     **/
    template<typename T> inline void relational_neuralgas<T>::setLogging( const bool& p )
    {
        m_logging = p;
    }
    
    
    
    /** shows the logging status
     * @return bool
     **/
    template<typename T> inline bool relational_neuralgas<T>::getLogging( void ) const
    {
        return m_logging;
    }
    
    
    
    /** returns every prototype step during training
     * @return std::vector with prototype matrix
     **/
    template<typename T> inline std::vector< ublas::matrix<T> > relational_neuralgas<T>::getLoggedPrototypes( void ) const
    {
        return m_logprototypes;
    }
    
    
    /** returns the dimension of prototypes
     * @return dimension of the prototypes
     **/
    template<typename T> inline std::size_t relational_neuralgas<T>::getPrototypeSize( void ) const 
    {
        return m_prototypes.size2();
    };
    
    
    /** returns the number of prototypes
     * @return number of the prototypes / classes
     **/
    template<typename T> inline std::size_t relational_neuralgas<T>::getPrototypeCount( void ) const 
    {
        return m_prototypes.size1();
    };
    
    
    /** returns the quantisation error 
     * @return error for each iteration
     **/
    template<typename T> inline std::vector<T> relational_neuralgas<T>::getLoggedQuantizationError( void ) const
    {
        return m_quantizationerror;
    }    
    
    
    /** 
     * train the prototypes
     * @param p_data data matrix
     * @param p_iterations number of iterations
     **/
    template<typename T> inline void relational_neuralgas<T>::train( const ublas::matrix<T>& p_data, const std::size_t& p_iterations )
    {
        train(p_data, p_iterations, m_prototypes.size1() * 0.5);
    }
    
    
    /** training the prototypes
     * @param p_data datapoints
     * @param p_iterations iterations
     * @param p_lambda max adapet size
     **/
    template<typename T> inline void relational_neuralgas<T>::train( const ublas::matrix<T>& p_data, const std::size_t& p_iterations, const T& p_lambda )
    {
        if (m_prototypes.size1() == 0)
            throw exception::runtime(_("number of prototypes must be greater than zero"));
        if (p_data.size1() < m_prototypes.size1())
            throw exception::runtime(_("number of datapoints are less than prototypes"));
        if (p_iterations == 0)
            throw exception::runtime(_("iterations must be greater than zero"));
        if (p_data.size2() != m_prototypes.size2())
            throw exception::runtime(_("data and prototype dimension are not equal"));
        if (p_lambda <= 0)
            throw exception::runtime(_("lambda must be greater than zero"));
        if (p_data.size1() != p_data.size2())
            throw exception::runtime(_("matrix must be square"));
        
        
        // creates logging
        if (m_logging) {
            m_logprototypes.clear();
            m_quantizationerror.clear();
            m_logprototypes.reserve(p_iterations);
            m_quantizationerror.reserve(p_iterations);
        }
        
        
        // run neural gas       
        const T l_multi = 0.01/p_lambda;
        
        for(std::size_t i=0; (i < p_iterations); ++i) {
            
            // create adapt values
            const T l_lambda = p_lambda * std::pow(l_multi, static_cast<T>(i)/static_cast<T>(p_iterations));
            
            // calculate for every prototype the distance
            // relational: (D * alpha_i)_j - 0.5 * alpha_i^t * D * alpha_i = || x^j - w^i || 
            // D = distance, alpha = weight of the prototype for the konvex combination
            ublas::matrix<T> l_adaptmatrix = ublas::prod(m_prototypes, p_data);
            
            for(std::size_t n=0; n < l_adaptmatrix.size1(); ++n) {
                const T l_val = 0.5 * ublas::inner_prod( ublas::row(m_prototypes, n), ublas::row(l_adaptmatrix, n) );
                
                for(std::size_t j=0; j < l_adaptmatrix.size2(); ++j)
                    l_adaptmatrix(n, j) -= l_val;
            }
            
            // determine quantization error for logging (adaption matrix)
            if (m_logging)
                m_quantizationerror.push_back( calculateQuantizationError(l_adaptmatrix) );

            
            // for every column ranks values and create adapts
            // we need rank and not randIndex, because we 
            // use the value of the ranking for calculate the 
            // adapt value
            for(std::size_t n=0; n < l_adaptmatrix.size2(); ++n) {
                ublas::vector<T> l_rank = ublas::column(l_adaptmatrix, n);
                l_rank = tools::vector::rank( l_rank );
                
                // calculate adapt value
                BOOST_FOREACH( T& p, l_rank)
                    p = std::exp( -p / l_lambda );
                
                // return value to matrix
                ublas::column(l_adaptmatrix, n) = l_rank;
            }
 
            // adapt values are the new prototypes (and run normalization)
            m_prototypes = l_adaptmatrix;
            for(std::size_t i=0; i < m_prototypes.size1(); ++i) {
                const T l_sum = ublas::sum( ublas::row( m_prototypes, i) );
                
                if (!tools::function::isNumericalZero(l_sum))
                    ublas::row( m_prototypes, i) /= l_sum;
            }
            
            // log updated prototypes
            if (m_logging)
                m_logprototypes.push_back( m_prototypes );
        }
    }
    
    
    /** calculate the quantization error
     * @param p_distance distance matrix (adaption matrix)
     * @return quantization error
     **/    
    template<typename T> inline T relational_neuralgas<T>::calculateQuantizationError( const ublas::matrix<T>& p_distance ) const
    {
        return 0.5 * ublas::sum( tools::matrix::min( p_distance, tools::matrix::column ) );
    }

    
    /** calulates distance between datapoints and prototypes and returns a std::vector
     * with index of the nearest prototype
     * @param p_data matrix
     * @return index array of prototype indices
     **/
    template<typename T> inline ublas::indirect_array<> relational_neuralgas<T>::use( const ublas::matrix<T>& p_data ) const
    {
        if (m_prototypes.size1() == 0)
            throw exception::runtime(_("number of prototypes must be greater than zero"));
        if (p_data.size2() != m_prototypes.size2())
            throw exception::runtime(_("data and prototype dimension are not equal"));
        
        ublas::indirect_array<> l_idx(p_data.size1());
        const ublas::matrix<T> l_distance = ublas::prod( m_prototypes, p_data );
        
        for(std::size_t i=0; i < l_distance.size2(); ++i) {
            ublas::vector<T> l_col                = ublas::column(l_distance, i);
            const ublas::indirect_array<> l_rank  = tools::vector::rankIndex( l_col );
            l_idx[i] = l_rank(0);
        }
        
        return l_idx;
    }
    
    //======= MPI ==================================================================================================================================
    #ifdef MACHINELEARNING_MPI
    
    /** returns the number of all prototypes
     * @param p_mpi MPI object for communication
     * @return number of prototypes
     **/
    template<typename T> inline std::size_t relational_neuralgas<T>::getNumberPrototypes( const mpi::communicator& p_mpi ) const
    {
        std::size_t l_count = 0;
        mpi::all_reduce(p_mpi, m_prototypes.size1(), l_count, std::plus<std::size_t>());
        return l_count;
    }
    
    
    /** train the data on the cluster
     * @param p_mpi MPI object for communication
     * @param p_data datapoints
     * @param p_iterations iterations
     **/
    template<typename T> inline void relational_neuralgas<T>::train( const mpi::communicator& p_mpi, const ublas::matrix<T>& p_data, const std::size_t& p_iterations )
    {
        // if the process has no prototypes, than lambda need not be zero, so we set it to a minimal numerical value, so the exception is not thrown
        train(p_mpi, p_data, p_iterations, ((m_prototypes.size1() == 0) ? std::numeric_limits<T>::epsilon() :  m_prototypes.size1() * 0.5) );
    }
    
    
    /** train the data on the cluster
     * @param p_mpi MPI object for communication
     * @param p_data datapoints
     * @param p_iterations iterations
     * @param p_lambda max adapet size
     **/
    template<typename T> inline void relational_neuralgas<T>::train( const mpi::communicator& p_mpi, const ublas::matrix<T>& p_data, const std::size_t& p_iterations, const T& p_lambda )
    {
    }
    
    #endif

};};};
#endif