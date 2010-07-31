#include "machinelearning.h"

#include <iostream>
#include <algorithm>

#include <boost/numeric/ublas/io.hpp>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>

#include <boost/numeric/bindings/ublas/matrix.hpp>
#include <boost/numeric/bindings/ublas/vector.hpp>



int main(int argc, char *argv[]) {

    namespace tl        = machinelearning::tools;
    namespace dist      = machinelearning::distances;
    namespace ndim      = machinelearning::dimensionreduce::nonsupervised;
    namespace dim       = machinelearning::dimensionreduce::supervised;
    namespace sl        = machinelearning::clustering::supervised;
    namespace nsl       = machinelearning::clustering::nonsupervised;
    namespace nd        = machinelearning::neighborhood;
        
    namespace linalg    = boost::numeric::bindings::lapack;
    namespace ublas     = boost::numeric::ublas;
    
    
    tl::files::hdf o("blub.hdf5");
    //std::cout << o.readString("/string") << std::endl;
	//std::vector<std::string> x = o.readStringVector("/stringarray");
    //ublas::vector<double> x = o.readVector<double>("/vector", H5::PredType::NATIVE_DOUBLE);
    //std::cout << x << std::endl;
    
    
    // ==== Spectral Clustering ====
    
    ublas::matrix<double> data = o.readMatrix<double>("/spectral2", H5::PredType::NATIVE_DOUBLE); 
    nsl::spectralclustering<double> sp(3, data.size1());
    sp.train(data, 15);
    
    tl::files::hdf f("spectral.hdf5", true);
    f.write<double>( "/data",  tl::matrix::setNumericalZero(sp.getPrototypes()), H5::PredType::NATIVE_DOUBLE );  
    
    
    // ==== LLE ====
    /*
    ublas::matrix<double> data = o.readMatrix<double>("/pcadata", H5::PredType::NATIVE_DOUBLE); 
    
    dist::euclid<double> d;
    nd::knn<double> k(d, 12);
    ndim::lle<double> l(k,2);
    
    tl::files::hdf f("lle.hdf5", true);
    f.write<double>( "/data",  l.map(data), H5::PredType::NATIVE_DOUBLE );  
    */
    
	
	// ==== LDA ====
    /*
    ublas::matrix<double> data = o.readMatrix<double>("/ldadata", H5::PredType::NATIVE_DOUBLE);
    std::vector<unsigned int> labels = tl::vector::copy( o.readVector<unsigned int>("/ldalabel", H5::PredType::NATIVE_UINT) );
    
    dim::lda<double, unsigned int> lx(2);
    tl::files::hdf f("lda.hdf5", true);
    f.write<double>( "/data",  lx.map(data, labels), H5::PredType::NATIVE_DOUBLE );    
    */
    
    
    // ==== PCA ====
    /*
    ublas::matrix<double> data = o.readMatrix<double>("/pcadata", H5::PredType::NATIVE_DOUBLE); 
    ndim::pca<double> p(2);
    
    tl::files::hdf f("pca.hdf5", true);
    f.write<double>( "/data",  p.map(data), H5::PredType::NATIVE_DOUBLE );
    */
    
    
    // ===== kmeans ===== 
    /*
    ublas::matrix<double> data = o.readMatrix<double>("/ngdata", H5::PredType::NATIVE_DOUBLE);
    
    dist::euclid<double> d;
    nsl::kmeans<double> kmeans(d, 11, 2);
    kmeans.setLogging(true);
    kmeans.train(data, 15);
    
    tl::files::hdf f("kmeans.hdf5", true);
    f.write<double>( "/protos",  kmeans.getPrototypes(), H5::PredType::NATIVE_DOUBLE );
    
    if (kmeans.getLogging()) {
        f.write<double>( "/error",  tl::vector::copy(kmeans.getLoggedQuantizationError()), H5::PredType::NATIVE_DOUBLE );
        std::vector< ublas::matrix<double> > p = kmeans.getLoggedPrototypes();
        for(std::size_t i=0; i < p.size(); ++i)
            f.write<double>("/log" + boost::lexical_cast<std::string>( i ), p[i], H5::PredType::NATIVE_DOUBLE );
    }*/
    
   
    // ===== NG ===== 
    /*
    ublas::matrix<double> data = o.readMatrix<double>("/ngdata", H5::PredType::NATIVE_DOUBLE);
     
    dist::euclid<double> d;
    nsl::neuralgas<double> ng(d, 11, 2);
    ng.setLogging(true);
    ng.train(data, 15);
    
    tl::files::hdf f("ng.hdf5", true);
    f.write<double>( "/protos",  ng.getPrototypes(), H5::PredType::NATIVE_DOUBLE );
    
    if (ng.getLogging()) {
        f.write<double>( "/error",  tl::vector::copy(ng.getLoggedQuantizationError()), H5::PredType::NATIVE_DOUBLE );
        std::vector< ublas::matrix<double> > p = ng.getLoggedPrototypes();
        for(std::size_t i=0; i < p.size(); ++i)
            f.write<double>("/log" + boost::lexical_cast<std::string>( i ), p[i], H5::PredType::NATIVE_DOUBLE );
    }
    */
    
    // ===== RLVQ ======
    /*
    ublas::matrix<double> data = o.readMatrix<double>("/rlvqdata", H5::PredType::NATIVE_DOUBLE);

    std::vector<unsigned int> lab(10);
    for (std::size_t i=0; i < lab.size(); ++i)
        lab[i] = i;
 
    dist::euclid<double> d;
    sl::rlvq<double, unsigned int> vq(d, lab, 64);
    vq.setLogging(true);
    
    
    std::vector<unsigned int> label = tl::vector::copy<unsigned int>( o.readVector<unsigned int>("rlvqlabel", H5::PredType::NATIVE_UINT) );

    vq.train(data, label, 10, 0.001, 0.0001);
    
    tl::files::hdf f("rlvq.hdf5", true);
    f.write<double>( "/protos",  vq.getPrototypes(), H5::PredType::NATIVE_DOUBLE );
    
    if (vq.getLogging()) {
        f.write<double>( "/error",  tl::vector::copy(vq.getLoggedQuantizationError()), H5::PredType::NATIVE_DOUBLE );
        std::vector< ublas::matrix<double> > p = vq.getLoggedPrototypes();
        for(std::size_t i=0; i < p.size(); ++i)
            f.write<double>("/log" + boost::lexical_cast<std::string>( i ), p[i], H5::PredType::NATIVE_DOUBLE );
    }
    */
     
    //=============== NCD ================
    /*
    std::vector< std:: string > val;
    val.push_back("main.cpp");
    val.push_back("tools/files/hdf.hpp");
    val.push_back("distances/ncd.hpp");
    val.push_back("clustering/supervised/rlvq.hpp");
    //val.push_back("blub.hdf5");
    
    
    dist::ncd ncd(dist::ncd::bzip2);
    std::cout << "unsymmetric: " << ncd.unsymmetric<double>(val, true) << std::endl;
    std::cout << "symmetric: " << ncd.symmetric<double>(val, true) << std::endl;
    */
    
    
    return EXIT_SUCCESS;
}
