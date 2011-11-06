/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#ifndef _TOOLKIT_CONTROLS_STDTABCONTROLLER_HXX_
#define _TOOLKIT_CONTROLS_STDTABCONTROLLER_HXX_


#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/awt/XTabController.hpp>
#include <com/sun/star/awt/XControl.hpp>
#include <com/sun/star/awt/XControlContainer.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <cppuhelper/weakagg.hxx>
#include <osl/mutex.hxx>
#include <toolkit/helper/macros.hxx>
#include <toolkit/helper/servicenames.hxx>


class StdTabController : 	public ::com::sun::star::awt::XTabController,
					        public ::com::sun::star::lang::XServiceInfo,
							public ::com::sun::star::lang::XTypeProvider,
							public ::cppu::OWeakAggObject
{
private:
	::osl::Mutex			maMutex;
	::com::sun::star::uno::Reference< ::com::sun::star::awt::XTabControllerModel >	mxModel;
	::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer > 	mxControlContainer;

protected:
	::osl::Mutex&				GetMutex() { return maMutex; }
	sal_Bool					ImplCreateComponentSequence( ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl > >& rControls, const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel > >& rModels, ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::awt::XWindow > >& rComponents, ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any>* pTabStops, sal_Bool bPeerComponent ) const;
	// wenn rModels kuerzer als rControls ist, werden nur die rModels entsprechenden Elemente geliefert und die korrespondierenden Elemente aus rControls entfernt
	void						ImplActivateControl( sal_Bool bFirst ) const;

public:
							StdTabController();
							~StdTabController();

	static ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl >	FindControl( ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl > >& rCtrls, const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlModel > & rxCtrlModel );

	// ::com::sun::star::uno::XInterface
	::com::sun::star::uno::Any	SAL_CALL queryInterface( const ::com::sun::star::uno::Type & rType ) throw(::com::sun::star::uno::RuntimeException) { return OWeakAggObject::queryInterface(rType); }
	void						SAL_CALL acquire() throw()	{ OWeakAggObject::acquire(); }
	void						SAL_CALL release() throw()	{ OWeakAggObject::release(); }

    ::com::sun::star::uno::Any	SAL_CALL queryAggregation( const ::com::sun::star::uno::Type & rType ) throw(::com::sun::star::uno::RuntimeException);

	// ::com::sun::star::lang::XTypeProvider
	::com::sun::star::uno::Sequence< ::com::sun::star::uno::Type >	SAL_CALL getTypes() throw(::com::sun::star::uno::RuntimeException);
	::com::sun::star::uno::Sequence< sal_Int8 >						SAL_CALL getImplementationId() throw(::com::sun::star::uno::RuntimeException);

	// XTabController
    void SAL_CALL setModel( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XTabControllerModel >& Model ) throw(::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XTabControllerModel > SAL_CALL getModel(  ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL setContainer( const ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer >& Container ) throw(::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControlContainer > SAL_CALL getContainer(  ) throw(::com::sun::star::uno::RuntimeException);
    ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Reference< ::com::sun::star::awt::XControl > > SAL_CALL getControls(  ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL autoTabOrder(  ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL activateTabOrder(  ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL activateFirst(  ) throw(::com::sun::star::uno::RuntimeException);
    void SAL_CALL activateLast(  ) throw(::com::sun::star::uno::RuntimeException);

    // XServiceInfo
	DECLIMPL_SERVICEINFO( StdTabController, szServiceName2_TabController )
};



#endif // _TOOLKIT_AWT_STDTABCONTROLLER_HXX_

