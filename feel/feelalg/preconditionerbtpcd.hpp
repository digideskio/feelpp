/* -*- mode: c++; coding: utf-8; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; show-trailing-whitespace: t  -*-
 
 This file is part of the Feel++ library
 
 Author(s): Christophe Prud'homme <christophe.prudhomme@feelpp.org>
            Goncalo Pena  <gpena@mat.uc.pt>
 Date: 02 Oct 2014
 
 Copyright (C) 2014 Feel++ Consortium
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
#ifndef FEELPP_PRECONDITIONERBTPCD_HPP
#define FEELPP_PRECONDITIONERBTPCD_HPP 1


#include <feel/feelalg/backend.hpp>
#include <feel/feelalg/operator.hpp>
#include <feel/feelalg/preconditioner.hpp>
#include <feel/feelalg/preconditionerpcd.hpp>

#include "preconditionerPCD.hpp"

namespace Feel
{
template< typename space_type >
class PreconditionerBTPCD : public Preconditioner<typename space_type::value>
{
public:

    typedef typename backend_type::sparse_matrix_type sparse_matrix_type;
    typedef typename backend_type::sparse_matrix_ptrtype sparse_matrix_ptrtype;

    typedef typename backend_type::vector_type vector_type;
    typedef typename backend_type::vector_ptrtype vector_ptrtype;

    typedef boost::shared_ptr<space_type> space_ptrtype;
    typedef typename space_type::mesh_type mesh_type;
    typedef typename space_type::mesh_ptrtype mesh_ptrtype;

    typedef typename space_type::template sub_functionspace<0>::type velocity_space_ptrtype;
    typedef typename space_type::template sub_functionspace<1>::type pressure_space_ptrtype;

    typedef typename velocity_space_ptrtype::element_type velocity_space_type;
    typedef typename pressure_space_ptrtype::element_type pressure_space_type;

    typedef typename velocity_space_type::element_type velocity_element_type;
    typedef typename pressure_space_type::element_type pressure_element_type;

    typedef typename space_type::value_type value_type;

    static const uint16_type Dim = space_type::nDim;
    static const uint16_type uOrder = velocity_space_type::basis_type::nOrder;
    static const uint16_type pOrder = pressure_space_type::basis_type::nOrder;

    typedef OperatorMatrix op_mat_type;
    typedef boost::shared_ptr<OperatorMatrix> op_mat_ptrtype;

    typedef PreconditionerPCD<pressure_space_type, uOrder> op_pcd_type;
    typedef boost::shared_ptr<op_pcd_type> op_pcd_ptrtype;

    PreconditionerBTPCD( space_ptrtype Xh, std::map<std::string, std::set<flag_type> > bcFlags, double nu, double alpha = 0 );

    PreconditionerBTPCD( const PreconditionerBTPCD& tc );

    void initialize();

    void assembleHelmholtz( double nu, double alpha = 0 );

    void assembleDivergence();

    void assembleGradient();

    void assembleSchurApp( double nu, double alpha = 0 );

    template< typename Expr_convection, typename Expr_bc >
    void update( Expr_convection const& expr_b, Expr_bc const& g, double& time2Update );

    int apply( const Epetra_MultiVector & X, Epetra_MultiVector & Y ) const
    {
        return (-1); //Not implemented
    }

    int applyInverse ( const Epetra_MultiVector& X, Epetra_MultiVector& Y ) const;

    // other function
    int SetUseTranspose( bool UseTranspose)
    {
        return(false);
    }

    double NormInf() const
    {
        return 0;
    }

    const char * Label () const
    {
        return("Triangular Block Preconditioner");
    }

    bool UseTranspose() const
    {
        return(false);
    }

    bool HasNormInf () const
    {
        return(false);
    }


    const Epetra_Comm & Comm() const
    {
        return( M_globalMap.Comm() );
    }

    const Epetra_Map & OperatorDomainMap() const
    {
        return( M_globalMap );
    }

    const Epetra_Map & OperatorRangeMap() const
    {
        return( M_globalMap );
    }

    ~PreconditionerBTPCD(){};

private:

    backend_ptrtype M_b;
    boost::shared_ptr<BackendTrilinos> M_bT;

    space_ptrtype M_Xh;

    velocity_space_ptrtype M_Vh;
    pressure_space_ptrtype M_Qh;

    Epetra_Map M_globalMap;

    sparse_matrix_ptrtype M_helm, G, M_div;
    op_mat_ptrtype divOp, helmOp;

    vector_ptrtype M_rhs;

    velocity_element_type u, v;
    pressure_element_type p, q;

    op_pcd_ptrtype pcdOp;

    std::map<std::string, std::set<flag_type> > M_bcFlags;

    operator_ptrtype precHelm;
};






template < typename space_type >
PreconditionerBTPCD<space_type>::PreconditionerBTPCD( space_ptrtype Xh, std::map<std::string, std::set<flag_type> > bcFlags,
                                                      double nu, double alpha )
    :
    M_Xh( Xh ),
    M_Vh( M_Xh->template functionSpace<0>() ),
    M_Qh( M_Xh->template functionSpace<1>() ),
    M_helm ( M_b->newMatrix( M_Vh, M_Vh ) ),
    G( M_b->newMatrix( M_Vh, M_Vh ) ),
    M_div ( M_b->newMatrix( M_Vh, M_Qh ) ),
    M_rhs( M_b->newVector( M_Vh )  ),
    u( M_Vh, "u" ),
    v( M_Vh, "v" ),
    p( M_Qh, "p" ),
    q( M_Qh, "q" ),
    M_bcFlags( bcFlags )
{
    mpi::timer ti;
    initialize();

    LOG(INFO) << "Assemble Helmholtz Operator...\n";
    assembleHelmholtz( nu, alpha );
    //LOG(INFO) << "[PreconditionerBTPCD] Constructor: Helmholtz assembly time: " << M_timer.ElapsedTime() << "\n";

    LOG(INFO) << "Assemble Divergence Operator...\n";
    assembleDivergence();
    //LOG(INFO) << "[PreconditionerBTPCD] Constructor: Divergence assembly time: " << M_timer.ElapsedTime() << "\n";

    LOG(INFO) << "Assemble Schur Operator...\n";
    assembleSchurApp(nu, alpha);
    //LOG(INFO) << "[PreconditionerBTPCD] Constructor: Schur approximation assembly time: " << M_timer.ElapsedTime() << "\n";
}



template < typename space_type >
PreconditionerBTPCD<space_type>::PreconditionerBTPCD( const PreconditionerBTPCD& tc )
    :
    M_Xh( tc.M_Xh ),
    M_Vh( tc.M_Vh ),
    M_Qh( tc.M_Qh ),
    M_helm( tc.M_helm ),
    G( tc.G ),
    M_div( tc.M_div ),
    divOp( tc.divOp ),
    helmOp( tc.helmOp ),
    M_rhs( tc.M_rhs ),
    u( tc.u ),
    v( tc.v ),
    q( tc.q ),
    pcdOp( tc.pcdOp ),
    M_bcFlags( tc.M_bcFlags ),
    precHelm( tc.precHelm )
{
    //Log() << "Call to Preconditioner BTPCD copy constructor...\n";
}

template < typename space_type >
void
PreconditionerBTPCD<space_type>::initialize()
{
    M_rhs->zero();
    M_rhs->close();
}

template < typename space_type >
void
PreconditionerBTPCD<space_type>::assembleHelmholtz( double nu, double alpha  )
{
    auto a = form2( _trial=M_Vh, _test=M_Vh, _matrix=M_helm );
    LOG(INFO) << "Alpha: " << alpha << "\n";
    if ( alpha != 0 )
        
        a = integrate( _range=elements(M_Xh->mesh()),  _expr=alpha*trans(idt(u))*id(v) + nu*(trace(trans(gradt(u))*grad(v))) );
    else
        a = integrate( _range=elements(M_Xh->mesh()),  _expr=nu*(trace(trans(gradt(u))*grad(v))) );

    std::set<flag_type>::const_iterator diriIter;
    for( auto dir : M_bcFlags["Dirichlet"] )
        {
            a += integrate( markedfaces(M_Xh->mesh(), dir), - nu*trans(gradt(u)*N())*id(v) );
        }

    M_helm->close();
}

template < typename space_type >
void
PreconditionerBTPCD<space_type>::assembleDivergence()
{
    auto a = form2( _trial=M_Vh, _test=M_Qh, _matrix=M_div );
    a = integrate( _range=elements(M_Xh->mesh()), _expr=divt( u ) * id( q ) );

    divOp = op( M_div, "DN");
}

template < typename space_type >
void
PreconditionerBTPCD<space_type>::assembleSchurApp( double nu, double alpha )
{
    pcdOp = pcd( M_Qh, M_bcFlags, nu, alpha );
}



template < typename space_type >
template< typename Expr_convection, typename Expr_bc >
void
PreconditionerBTPCD<space_type>::update( Expr_convection const& expr_b, Expr_bc const& g, double& time2Update )
{
    static bool init_G = true;

    boost::timer ti;

    if ( !init_G )
        G->zero();

    auto g = form2( _trial=M_Vh, _test=M_Vh, _matrix=G );
    
    g = integrate( elements(M_Xh->mesh()),
                   trans( gradt(u)*val(expr_b) )*id(v)
                   );

    G->close();

    G->addMatrix( 1.0, M_helm );

    std::set<flag_type>::const_iterator diriIter;
    for( auto dir : M_bcFlags["Dirichlet"] )
    {
        a += on( markedfaces(M_Xh->mesh(), dir ), u, M_rhs, g, ON_ELIMINATION );
    }

    helmOp = op( G, "HN" );

    ti.restart();
    pcdOp->update( expr_b );
    time2Update += ti.elapsed();

    init_G = false;
}



template < typename space_type >
int
PreconditionerBTPCD<space_type>::applyInverse ( const vector_type& X, vector_type& Y ) const
{
    //LOG(INFO) << "Create velocity component...\n";
    vector_type velocity_in ( getComponent<0>(M_Xh, X) );
    
    vector_type pressure_in ( getComponent<1>(M_Xh, X) );

    //LOG(INFO) << "Copy velocity component...\n";
    vector_type velocity_out( velocity_in );

    //LOG(INFO) << "Copy pressure component...\n";
    vector_type pressure_out( pressure_in);


    //LOG(INFO) << "apply inverse helmholtz...\n";
    helmOp->applyInverse(velocity_in, velocity_out);

    //LOG(INFO) << "apply divergence...\n";
    vector_type aux( pressure_in );
    divOp->apply( velocity_out, aux );

    pressure_in.Update( 1.0, aux, -1.0);

    //LOG(INFO) << "Solve for the pressure...\n";
    pcdOp->applyInverse(pressure_in, pressure_out);

    //LOG(INFO) << "Update pressure...\n";
    UpdateComponent<1>(M_Xh, Y, pressure_out);

    //LOG(INFO) << "Update velocity...\n";
    UpdateComponent<0>(M_Xh, Y, velocity_out);

    return 0;
}

} // Feel
#endif