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



#ifndef _DBA_CORE_VIEWCONTAINER_HXX_
#define _DBA_CORE_VIEWCONTAINER_HXX_

#ifndef _CPPUHELPER_IMPLBASE1_HXX_
#include <cppuhelper/implbase1.hxx>
#endif
#ifndef _COMPHELPER_STLTYPES_HXX_
#include <comphelper/stl_types.hxx>
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XENUMERATIONACCESS_HPP_
#include <com/sun/star/container/XEnumerationAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XINDEXACCESS_HPP_
#include <com/sun/star/container/XIndexAccess.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XREFRESHABLE_HPP_
#include <com/sun/star/util/XRefreshable.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_UNO_SEQUENCE_HXX_
#include <com/sun/star/uno/Sequence.hxx>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_SQLWARNING_HPP_
#include <com/sun/star/sdbc/SQLWarning.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_SQLCONTEXT_HPP_
#include <com/sun/star/sdb/SQLContext.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XCONTAINERLISTENER_HPP_
#include <com/sun/star/container/XContainerListener.hpp>
#endif
#ifndef _DBASHARED_APITOOLS_HXX_
#include "apitools.hxx"
#endif

#ifndef DBACCESS_CORE_FILTERED_CONTAINER_HXX
#include "FilteredContainer.hxx"
#endif

namespace dbtools
{
	class IWarningsContainer;
}

namespace dbaccess
{
	typedef ::cppu::ImplHelper1< ::com::sun::star::container::XContainerListener> OViewContainer_Base;

	//==========================================================================
	//= OViewContainer
	//==========================================================================
	class OViewContainer :  public OFilteredContainer,
							public OViewContainer_Base
	{
	public:
		/** ctor of the container. The parent has to support the <type scope="com::sun::star::sdbc">XConnection</type>
			interface.<BR>
			@param			_rParent			the object which acts as parent for the container.
												all refcounting is rerouted to this object
			@param			_rMutex				the access safety object of the parent
			@param			_rTableFilter		restricts the visible tables by name
			@param			_rTableTypeFilter	restricts the visible tables by type
			@see			construct
		*/
		OViewContainer( ::cppu::OWeakObject& _rParent,
						::osl::Mutex& _rMutex,
						const ::com::sun::star::uno::Reference< ::com::sun::star::sdbc::XConnection >& _xCon,
						sal_Bool _bCase,
						IRefreshListener*	_pRefreshListener,
						::dbtools::IWarningsContainer* _pWarningsContainer,
                        oslInterlockedCount& _nInAppend
						);

        virtual ~OViewContainer();		

    protected:
        // OFilteredContainer overridables
        virtual ::rtl::OUString getTableTypeRestriction() const;

    private:
		inline virtual void SAL_CALL acquire() throw(){ OFilteredContainer::acquire();}
		inline virtual void SAL_CALL release() throw(){ OFilteredContainer::release();}
	// ::com::sun::star::lang::XServiceInfo
		DECLARE_SERVICE_INFO();

		// XEventListener
		virtual void SAL_CALL disposing( const ::com::sun::star::lang::EventObject& Source ) throw (::com::sun::star::uno::RuntimeException);
		// XContainerListener
		virtual void SAL_CALL elementInserted( const ::com::sun::star::container::ContainerEvent& Event ) throw (::com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL elementRemoved( const ::com::sun::star::container::ContainerEvent& Event ) throw (::com::sun::star::uno::RuntimeException);
		virtual void SAL_CALL elementReplaced( const ::com::sun::star::container::ContainerEvent& Event ) throw (::com::sun::star::uno::RuntimeException);

		// ::connectivity::sdbcx::OCollection
		virtual ::connectivity::sdbcx::ObjectType		createObject(const ::rtl::OUString& _rName);
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >	createDescriptor();
        virtual connectivity::sdbcx::ObjectType appendObject( const ::rtl::OUString& _rForName, const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& descriptor );
		virtual void dropObject(sal_Int32 _nPos,const ::rtl::OUString _sElementName);

        using OFilteredContainer::disposing;

        bool m_bInElementRemoved;
	};
}
#endif // _DBA_CORE_VIEWCONTAINER_HXX_


