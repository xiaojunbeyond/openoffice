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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_dbaccess.hxx"

#ifndef _DBAUI_SQLMESSAGE_HXX_
#include "sqlmessage.hxx"
#endif
#ifndef _DBU_DLG_HRC_
#include "dbu_dlg.hrc"
#endif
#ifndef _DBAUI_SQLMESSAGE_HRC_
#include "sqlmessage.hrc"
#endif
#ifndef _COM_SUN_STAR_SDBC_SQLEXCEPTION_HPP_
#include <com/sun/star/sdbc/SQLException.hpp>
#endif
#ifndef _COM_SUN_STAR_SDB_SQLCONTEXT_HPP_
#include <com/sun/star/sdb/SQLContext.hpp>
#endif
#ifndef _SV_GROUP_HXX //autogen
#include <vcl/fixed.hxx>
#endif
#ifndef _SVTREEBOX_HXX
#include <svtools/svtreebx.hxx>
#endif
#ifndef _SVEDIT_HXX //autogen
#include <svtools/svmedit.hxx>
#endif
#ifndef _DBHELPER_DBEXCEPTION_HXX_
#include <connectivity/dbexception.hxx>
#endif
#ifndef CONNECTIVITY_SQLERROR_HXX
#include <connectivity/sqlerror.hxx>
#endif
#ifndef _SV_MSGBOX_HXX //autogen
#include <vcl/msgbox.hxx>
#endif
#ifndef _UTL_CONFIGMGR_HXX_
#include <unotools/configmgr.hxx>
#endif
#ifndef _SFX_SFXUNO_HXX
#include <sfx2/sfxuno.hxx>
#endif
#ifndef _DBA_DBACCESS_HELPID_HRC_
#include "dbaccess_helpid.hrc"
#endif
#ifndef DBAUI_TOOLS_HXX
#include "UITools.hxx"
#endif
#ifndef _DBAUI_MODULE_DBU_HXX_
#include "moduledbu.hxx"
#endif

#include <tools/urlobj.hxx>

#define BUTTONID_MORE	BUTTONID_RETRY + 1

#define DIALOG_WIDTH    220
#define OUTER_MARGIN    6
#define IMAGE_SIZE      20
#define INNER_PADDING   3
#define TEXT_POS_X      ( OUTER_MARGIN + IMAGE_SIZE + INNER_PADDING )

using namespace dbtools;
using namespace com::sun::star::uno;
using namespace com::sun::star::sdb;
using namespace com::sun::star::sdbc;

//.........................................................................
namespace dbaui
{
//.........................................................................

namespace
{
    //------------------------------------------------------------------------------
    class IImageProvider
    {
    public:
        virtual Image   getImage( bool _highContrast ) const = 0;

        virtual ~IImageProvider() { }
    };

    //------------------------------------------------------------------------------
    class ILabelProvider
    {
    public:
        virtual String  getLabel() const = 0;

        virtual ~ILabelProvider() { };
    };

    //------------------------------------------------------------------------------
    class ImageProvider : public IImageProvider
    {
    private:
        sal_uInt16  m_defaultImageID;
        sal_uInt16  m_highContrastImageID;

        mutable Image   m_defaultImage;
        mutable Image   m_highContrastImage;

    public:
        ImageProvider( sal_uInt16 _defaultImageID, sal_uInt16 _highContrastImageID )
            :m_defaultImageID( _defaultImageID )
            ,m_highContrastImageID( _highContrastImageID )
        {
        }

        virtual Image getImage( bool _highContrast ) const
        {
            if ( _highContrast )
            {
                if ( !m_highContrastImage )
                    m_highContrastImage = Image( ModuleRes( m_highContrastImageID ) );
                return m_highContrastImage;
            }

            if ( !m_defaultImage )
                m_defaultImage = Image( ModuleRes( m_defaultImageID ) );
            return m_defaultImage;
        }
    };

    //------------------------------------------------------------------------------
    class LabelProvider : public ILabelProvider
    {
    private:
        String  m_label;
    public:
        LabelProvider( sal_uInt16 _labelResourceID )
            :m_label( ModuleRes( _labelResourceID ) )
        {
        }
    
        virtual String  getLabel() const
        {
            return m_label;
        }
    };

    //------------------------------------------------------------------------------
    class ProviderFactory
    {
    private:
        mutable ::boost::shared_ptr< IImageProvider >   m_pErrorImage;
        mutable ::boost::shared_ptr< IImageProvider >   m_pWarningsImage;
        mutable ::boost::shared_ptr< IImageProvider >   m_pInfoImage;
        mutable ::boost::shared_ptr< ILabelProvider >   m_pErrorLabel;
        mutable ::boost::shared_ptr< ILabelProvider >   m_pWarningsLabel;
        mutable ::boost::shared_ptr< ILabelProvider >   m_pInfoLabel;

    public:
        ProviderFactory()
        {
        }

        ::boost::shared_ptr< IImageProvider >   getImageProvider( SQLExceptionInfo::TYPE _eType ) const
        {
            ::boost::shared_ptr< IImageProvider >* ppProvider( &m_pErrorImage );
            sal_uInt16 nNormalImageID( BMP_EXCEPTION_ERROR );
            sal_uInt16 nHCImageID( BMP_EXCEPTION_ERROR_SCH );

            switch ( _eType )
            {
            case SQLExceptionInfo::SQL_WARNING:
                ppProvider = &m_pWarningsImage;
                nNormalImageID = BMP_EXCEPTION_WARNING;
                nHCImageID = BMP_EXCEPTION_WARNING_SCH;
                break;

            case SQLExceptionInfo::SQL_CONTEXT:
                ppProvider = &m_pInfoImage;
                nNormalImageID = BMP_EXCEPTION_INFO;
                nHCImageID = BMP_EXCEPTION_INFO_SCH;
                break;

            default:
                break;
            }

            if ( !ppProvider->get() )
                ppProvider->reset( new ImageProvider( nNormalImageID, nHCImageID ) );
            return *ppProvider;
        }

        ::boost::shared_ptr< ILabelProvider >   getLabelProvider( SQLExceptionInfo::TYPE _eType, bool _bSubLabel ) const
        {
            ::boost::shared_ptr< ILabelProvider >* ppProvider( &m_pErrorLabel );
            sal_uInt16 nLabelID( STR_EXCEPTION_ERROR );

            switch ( _eType )
            {
            case SQLExceptionInfo::SQL_WARNING:
                ppProvider = &m_pWarningsLabel;
                nLabelID = STR_EXCEPTION_WARNING;
                break;

            case SQLExceptionInfo::SQL_CONTEXT:
                ppProvider = &m_pInfoLabel;
                nLabelID = _bSubLabel ? STR_EXCEPTION_DETAILS : STR_EXCEPTION_INFO;
                break;
            default:
                break;
            }

            if ( !ppProvider->get() )
                ppProvider->reset( new LabelProvider( nLabelID ) );
            return *ppProvider;
        }

    };

    //------------------------------------------------------------------------------
    /// a stripped version of the SQLException, packed for displaying
    struct ExceptionDisplayInfo
    {
        SQLExceptionInfo::TYPE                  eType;

        ::boost::shared_ptr< IImageProvider >   pImageProvider;
        ::boost::shared_ptr< ILabelProvider >   pLabelProvider;

        bool                                    bSubEntry;

        String                                  sMessage;
        String                                  sSQLState;
        String                                  sErrorCode;

        ExceptionDisplayInfo() : eType( SQLExceptionInfo::UNDEFINED ), bSubEntry( false ) { }
        ExceptionDisplayInfo( SQLExceptionInfo::TYPE _eType ) : eType( _eType ), bSubEntry( false ) { }
    };

    static bool lcl_hasDetails( const ExceptionDisplayInfo& _displayInfo )
    {
        return  ( _displayInfo.sErrorCode.Len() )
                ||  (   _displayInfo.sSQLState.Len()
                    &&  !_displayInfo.sSQLState.EqualsAscii( "S1000" )
                    );
    }

    typedef ::std::vector< ExceptionDisplayInfo >   ExceptionDisplayChain;

    //------------------------------------------------------------------------------
    /// strips the [OOoBase] vendor identifier from the given error message, if applicable
    ::rtl::OUString lcl_stripOOoBaseVendor( const ::rtl::OUString& _rErrorMessage )
    {
        ::rtl::OUString sErrorMessage( _rErrorMessage );

        const ::rtl::OUString sVendorIdentifier( ::connectivity::SQLError::getMessagePrefix() );
        if ( sErrorMessage.indexOf( sVendorIdentifier ) == 0 )
        {
            // characters to strip
            sal_Int32 nStripLen( sVendorIdentifier.getLength() );
            // usually, there should be a whitespace between the vendor and the real message
            while   (   ( sErrorMessage.getLength() > nStripLen )
                    &&  ( sErrorMessage[nStripLen] == ' ' )
                    )
                    ++nStripLen;
            sErrorMessage = sErrorMessage.copy( nStripLen );
        }

        return sErrorMessage;
    }

    //------------------------------------------------------------------------------
    void lcl_buildExceptionChain( const SQLExceptionInfo& _rErrorInfo, const ProviderFactory& _rFactory, ExceptionDisplayChain& _out_rChain )
    {
        {
            ExceptionDisplayChain empty;
            _out_rChain.swap( empty );
        }

        SQLExceptionIteratorHelper iter( _rErrorInfo );
        while ( iter.hasMoreElements() )
        {
            // current chain element
            SQLExceptionInfo aCurrentElement;
            iter.next( aCurrentElement );

            const SQLException* pCurrentError = (const SQLException*)aCurrentElement;
            DBG_ASSERT( pCurrentError, "lcl_buildExceptionChain: iterator failure!" );
                // hasMoreElements should not have returned <TRUE/> in this case

            ExceptionDisplayInfo aDisplayInfo( aCurrentElement.getType() );

            aDisplayInfo.sMessage = pCurrentError->Message.trim();
            aDisplayInfo.sSQLState = pCurrentError->SQLState;
            if ( pCurrentError->ErrorCode )
                aDisplayInfo.sErrorCode = String::CreateFromInt32( pCurrentError->ErrorCode );

            if  (   !aDisplayInfo.sMessage.Len()
                &&  !lcl_hasDetails( aDisplayInfo )
                )
            {
                OSL_ENSURE( false, "lcl_buildExceptionChain: useles exception: no state, no error code, no message!" );
                continue;
            }

            aDisplayInfo.pImageProvider = _rFactory.getImageProvider( aCurrentElement.getType() );
            aDisplayInfo.pLabelProvider = _rFactory.getLabelProvider( aCurrentElement.getType(), false );

            _out_rChain.push_back( aDisplayInfo );

            if ( aCurrentElement.getType() == SQLExceptionInfo::SQL_CONTEXT )
            {
                const SQLContext* pContext = (const SQLContext*)aCurrentElement;
                if ( pContext->Details.getLength() )
                {
                    ExceptionDisplayInfo aSubInfo( aCurrentElement.getType() );

                    aSubInfo.sMessage = pContext->Details;
                    aSubInfo.pImageProvider = _rFactory.getImageProvider( aCurrentElement.getType() );
                    aSubInfo.pLabelProvider = _rFactory.getLabelProvider( aCurrentElement.getType(), true );
                    aSubInfo.bSubEntry = true;

                    _out_rChain.push_back( aSubInfo );
                }
            }
        }
    }

    //------------------------------------------------------------------------------
    void lcl_insertExceptionEntry( SvTreeListBox& _rList, bool _bHiContrast, size_t _nElementPos, const ExceptionDisplayInfo& _rEntry )
    {
        Image aEntryImage( _rEntry.pImageProvider->getImage( _bHiContrast ) );
        SvLBoxEntry* pListEntry =
            _rList.InsertEntry( _rEntry.pLabelProvider->getLabel(), aEntryImage, aEntryImage );
        pListEntry->SetUserData( reinterpret_cast< void* >( _nElementPos ) );
    }
}

//==============================================================================
class OExceptionChainDialog : public ModalDialog
{
	FixedLine       m_aFrame;
	FixedText		m_aListLabel;
	SvTreeListBox	m_aExceptionList;
	FixedText		m_aDescLabel;
	MultiLineEdit	m_aExceptionText;
	OKButton		m_aOK;

    String          m_sStatusLabel;
    String          m_sErrorCodeLabel;

    ExceptionDisplayChain   m_aExceptions;

public:
	OExceptionChainDialog( Window* pParent, const ExceptionDisplayChain& _rExceptions );
	~OExceptionChainDialog();

protected:
	DECL_LINK(OnExceptionSelected, void*);
};

DBG_NAME(OExceptionChainDialog)
//------------------------------------------------------------------------------
OExceptionChainDialog::OExceptionChainDialog( Window* pParent, const ExceptionDisplayChain& _rExceptions )
	:ModalDialog(pParent, ModuleRes(DLG_SQLEXCEPTIONCHAIN))
	,m_aFrame			(this, ModuleRes(FL_DETAILS))
	,m_aListLabel		(this, ModuleRes(FT_ERRORLIST))
	,m_aExceptionList	(this, ModuleRes(CTL_ERRORLIST))
	,m_aDescLabel		(this, ModuleRes(FT_DESCRIPTION))
	,m_aExceptionText	(this, ModuleRes(ME_DESCRIPTION))
	,m_aOK				(this, ModuleRes(PB_OK))
    ,m_aExceptions( _rExceptions )
{
    DBG_CTOR(OExceptionChainDialog,NULL);

    m_sStatusLabel = String( ModuleRes( STR_EXCEPTION_STATUS ) );
	m_sErrorCodeLabel = String( ModuleRes( STR_EXCEPTION_ERRORCODE ) );

	FreeResource();

	m_aExceptionList.SetSelectionMode(SINGLE_SELECTION);
	m_aExceptionList.SetDragDropMode(0);
	m_aExceptionList.EnableInplaceEditing(sal_False);
	m_aExceptionList.SetStyle(m_aExceptionList.GetStyle() | WB_HASLINES | WB_HASBUTTONS | WB_HASBUTTONSATROOT | WB_HSCROLL);

	m_aExceptionList.SetSelectHdl(LINK(this, OExceptionChainDialog, OnExceptionSelected));
	m_aExceptionList.SetNodeDefaultImages( );
	m_aExceptionText.SetReadOnly(sal_True);

    bool bHave22018 = false;
	bool bHiContrast = isHiContrast( this );
    size_t elementPos = 0;

    for (   ExceptionDisplayChain::const_iterator loop = m_aExceptions.begin();
            loop != m_aExceptions.end();
            ++loop, ++elementPos
        )
    {
        lcl_insertExceptionEntry( m_aExceptionList, bHiContrast, elementPos, *loop );
        bHave22018 = loop->sSQLState.EqualsAscii( "22018" );
    }

    // if the error has the code 22018, then add an additional explanation
    // #i24021# / 2004-10-14 / frank.schoenheit@sun.com
    if ( bHave22018 )
    {
        ProviderFactory aProviderFactory;

        ExceptionDisplayInfo aInfo22018;
        aInfo22018.sMessage = String( ModuleRes( STR_EXPLAN_STRINGCONVERSION_ERROR ) );
        aInfo22018.pLabelProvider = aProviderFactory.getLabelProvider( SQLExceptionInfo::SQL_CONTEXT, false );
        aInfo22018.pImageProvider = aProviderFactory.getImageProvider( SQLExceptionInfo::SQL_CONTEXT );
        m_aExceptions.push_back( aInfo22018 );

        lcl_insertExceptionEntry( m_aExceptionList, bHiContrast, m_aExceptions.size() - 1, aInfo22018 );
    }
}

//------------------------------------------------------------------------------
OExceptionChainDialog::~OExceptionChainDialog()
{
    DBG_DTOR(OExceptionChainDialog,NULL);
}

//------------------------------------------------------------------------------
IMPL_LINK(OExceptionChainDialog, OnExceptionSelected, void*, EMPTYARG)
{
	SvLBoxEntry* pSelected = m_aExceptionList.FirstSelected();
	DBG_ASSERT(!pSelected || !m_aExceptionList.NextSelected(pSelected), "OExceptionChainDialog::OnExceptionSelected : multi selection ?");

    String sText;

	if ( pSelected )
	{
        size_t pos = reinterpret_cast< size_t >( pSelected->GetUserData() );
        const ExceptionDisplayInfo& aExceptionInfo( m_aExceptions[ pos ] );

        if ( aExceptionInfo.sSQLState.Len() )
        {
            sText += m_sStatusLabel;
			sText.AppendAscii(": ");
			sText += aExceptionInfo.sSQLState;
			sText.AppendAscii("\n");
        }

        if ( aExceptionInfo.sErrorCode.Len() )
		{
			sText += m_sErrorCodeLabel;
			sText.AppendAscii(": ");
			sText += aExceptionInfo.sErrorCode;
			sText.AppendAscii("\n");
		}

        if ( sText.Len() )
            sText.AppendAscii( "\n" );

        sText += aExceptionInfo.sMessage;
	}

    m_aExceptionText.SetText(sText);

    return 0L;
}

//==============================================================================
//= SQLMessageBox_Impl
//==============================================================================
struct SQLMessageBox_Impl
{
    ExceptionDisplayChain   aDisplayInfo;

    SQLMessageBox_Impl( const SQLExceptionInfo& _rExceptionInfo )
    {
        // transform the exception chain to a form more suitable for displaying it here
        ProviderFactory aProviderFactory;
        lcl_buildExceptionChain( _rExceptionInfo, aProviderFactory, aDisplayInfo );
    }
};

//------------------------------------------------------------------------------
namespace
{
    ::rtl::OUString lcl_getProductName()
    {
    	::rtl::OUString sProductName;
	    OSL_VERIFY( ::utl::ConfigManager::GetDirectConfigProperty( ::utl::ConfigManager::PRODUCTNAME ) >>= sProductName );
        return sProductName;
    }

    void lcl_positionInAppFont( const Window& _rParent, Window& _rChild, long _nX, long _nY, long _Width, long _Height )
    {
        Point aPos = _rParent.LogicToPixel( Point( _nX, _nY ), MAP_APPFONT );
        Size aSize = _rParent.LogicToPixel( Size( _Width, _Height ), MAP_APPFONT );
        _rChild.SetPosSizePixel( aPos, aSize );
    }

    void lcl_addButton( ButtonDialog& _rDialog, StandardButtonType _eType, bool _bDefault )
    {
        sal_uInt16 nButtonID = 0;
        switch ( _eType )
        {
        case BUTTON_YES:    nButtonID = BUTTONID_YES; break;
        case BUTTON_NO:     nButtonID = BUTTONID_NO; break;
        case BUTTON_OK:     nButtonID = BUTTONID_OK; break;
        case BUTTON_CANCEL: nButtonID = BUTTONID_CANCEL; break;
        case BUTTON_RETRY:  nButtonID = BUTTONID_RETRY; break;
        case BUTTON_HELP:   nButtonID = BUTTONID_HELP; break;
        default:
            OSL_ENSURE( false, "lcl_addButton: invalid button id!" );
            break;
        }
        _rDialog.AddButton( _eType, nButtonID, _bDefault ? BUTTONDIALOG_DEFBUTTON | BUTTONDIALOG_FOCUSBUTTON : 0 );
    }
}

//------------------------------------------------------------------------------
void OSQLMessageBox::impl_positionControls()
{
    OSL_PRECOND( !m_pImpl->aDisplayInfo.empty(), "OSQLMessageBox::impl_positionControls: nothing to display at all?" );

    
    if ( m_pImpl->aDisplayInfo.empty() )
        return;
    const ExceptionDisplayInfo* pSecondInfo = NULL;
    
    const ExceptionDisplayInfo& rFirstInfo = *m_pImpl->aDisplayInfo.begin();
    if ( m_pImpl->aDisplayInfo.size() > 1 )
        pSecondInfo = &m_pImpl->aDisplayInfo[1];
    String sPrimary, sSecondary;
    sPrimary = rFirstInfo.sMessage;
    // one or two texts to display?
    if ( pSecondInfo )
    {
        // we show two elements in the main dialog if and only if one of
        // - the first element in the chain is an SQLContext, and the second
        //   element denotes its sub entry
        // - the first and the second element are both independent (i.e. the second
        //   is no sub entry), and none of them is a context.
        bool bFirstElementIsContext = ( rFirstInfo.eType == SQLExceptionInfo::SQL_CONTEXT );
        bool bSecondElementIsContext = ( pSecondInfo->eType == SQLExceptionInfo::SQL_CONTEXT );

        if ( bFirstElementIsContext && pSecondInfo->bSubEntry )
            sSecondary = pSecondInfo->sMessage;
        if ( !bFirstElementIsContext && !bSecondElementIsContext )
            sSecondary = pSecondInfo->sMessage;
    }

    // image
    lcl_positionInAppFont( *this, m_aInfoImage, OUTER_MARGIN, OUTER_MARGIN, IMAGE_SIZE, IMAGE_SIZE );
	m_aInfoImage.Show();

    // primary text
    lcl_positionInAppFont( *this, m_aTitle, TEXT_POS_X, OUTER_MARGIN, DIALOG_WIDTH - TEXT_POS_X - 2 * OUTER_MARGIN, 16 );
    sPrimary = lcl_stripOOoBaseVendor( sPrimary );
    m_aTitle.SetText( sPrimary );
	m_aTitle.Show();

    Rectangle aPrimaryRect( m_aTitle.GetPosPixel(), m_aTitle.GetSizePixel() );

    // secondary text (if applicable)
	m_aMessage.SetStyle( m_aMessage.GetStyle() | WB_NOLABEL );
    sSecondary = lcl_stripOOoBaseVendor( sSecondary );
    m_aMessage.SetText( sSecondary );

    lcl_positionInAppFont( *this, m_aMessage, TEXT_POS_X, OUTER_MARGIN + 16 + 3, DIALOG_WIDTH - TEXT_POS_X - 2 * OUTER_MARGIN, 8 );
    Rectangle aSecondaryRect( m_aMessage.GetPosPixel(), m_aMessage.GetSizePixel() );

    bool bHaveSecondaryText = sSecondary.Len() != 0;

    // determine which space the secondary text would occupy
    if ( bHaveSecondaryText )
	    aSecondaryRect = GetTextRect( aSecondaryRect, sSecondary, TEXT_DRAW_WORDBREAK | TEXT_DRAW_MULTILINE | TEXT_DRAW_LEFT );
    else
        aSecondaryRect.Bottom() = aSecondaryRect.Top() - 1;

    // adjust secondary control height accordingly
    m_aMessage.SetSizePixel( aSecondaryRect.GetSize() );
	m_aMessage.Show( aSecondaryRect.GetHeight() > 0 );

    // if there's no secondary text ...
    if ( !bHaveSecondaryText )
    {   // then give the primary text as much horizontal space as it needs
        Rectangle aSuggestedRect( GetTextRect( aPrimaryRect, sPrimary, TEXT_DRAW_WORDBREAK | TEXT_DRAW_MULTILINE | TEXT_DRAW_CENTER ) );
        aPrimaryRect.Right() = aPrimaryRect.Left() + aSuggestedRect.GetWidth();
        aPrimaryRect.Bottom() = aPrimaryRect.Top() + aSuggestedRect.GetHeight();
        // and center it horizontally
        m_aTitle.SetStyle( ( m_aTitle.GetStyle() & ~WB_LEFT ) | WB_CENTER );

        Rectangle aInfoRect( m_aInfoImage.GetPosPixel(), m_aInfoImage.GetSizePixel() );
        // also, if it's not as high as the image ...
        if ( aPrimaryRect.GetHeight() < m_aInfoImage.GetSizePixel().Height() )
        {   // ... make it fit the image height
            aPrimaryRect.Bottom() += aInfoRect.GetHeight() - aPrimaryRect.GetHeight();
            // and center it vertically
            m_aTitle.SetStyle( m_aTitle.GetStyle() | WB_VCENTER );
        }
        else
        {   // ... otherwise, center the image vertically, relative to the primary text
            aInfoRect.Move( 0, ( aPrimaryRect.GetHeight() - aInfoRect.GetHeight() ) / 2 );
            m_aInfoImage.SetPosSizePixel( aInfoRect.TopLeft(), aInfoRect.GetSize() );
        }

        m_aTitle.SetPosSizePixel( aPrimaryRect.TopLeft(), aPrimaryRect.GetSize() );
    }

    // adjust dialog size accordingly
    const Rectangle& rBottomTextRect( bHaveSecondaryText ? aSecondaryRect : aPrimaryRect );
    Size aBorderSize = LogicToPixel( Size( OUTER_MARGIN, OUTER_MARGIN ), MAP_APPFONT );
    Size aDialogSize( LogicToPixel( Size( DIALOG_WIDTH, 30 ), MAP_APPFONT ) );
    aDialogSize.Height() = rBottomTextRect.Bottom() + aBorderSize.Height();
    aDialogSize.Width() = aPrimaryRect.Right() + aBorderSize.Width();

	SetSizePixel( aDialogSize );
	SetPageSizePixel( aDialogSize );
}

//------------------------------------------------------------------------------
void OSQLMessageBox::impl_initImage( MessageType _eImage )
{
	switch (_eImage)
	{
        default:
            DBG_ERROR( "OSQLMessageBox::impl_initImage: unsupported image type!" );

		case Info:
			m_aInfoImage.SetImage(InfoBox::GetStandardImage());
			break;
		case Warning:
			m_aInfoImage.SetImage(WarningBox::GetStandardImage());
			break;
		case Error:
			m_aInfoImage.SetImage(ErrorBox::GetStandardImage());
			break;
		case Query:
			m_aInfoImage.SetImage(QueryBox::GetStandardImage());
			break;
	}
}

//------------------------------------------------------------------------------
void OSQLMessageBox::impl_createStandardButtons( WinBits _nStyle )
{
	if ( _nStyle & WB_YES_NO_CANCEL )
	{
        lcl_addButton( *this, BUTTON_YES,    ( _nStyle & WB_DEF_YES ) != 0 );
        lcl_addButton( *this, BUTTON_NO,     ( _nStyle & WB_DEF_NO ) != 0 );
        lcl_addButton( *this, BUTTON_CANCEL, ( _nStyle & WB_DEF_CANCEL ) != 0 );
	}
	else if ( _nStyle & WB_OK_CANCEL )
	{
        lcl_addButton( *this, BUTTON_OK,     ( _nStyle & WB_DEF_OK ) != 0 );
        lcl_addButton( *this, BUTTON_CANCEL, ( _nStyle & WB_DEF_CANCEL ) != 0 );
	}
	else if ( _nStyle & WB_YES_NO )
	{
        lcl_addButton( *this, BUTTON_YES,    ( _nStyle & WB_DEF_YES ) != 0 );
        lcl_addButton( *this, BUTTON_NO,     ( _nStyle & WB_DEF_NO ) != 0 );
	}
	else if ( _nStyle & WB_RETRY_CANCEL )
	{
        lcl_addButton( *this, BUTTON_RETRY,  ( _nStyle & WB_DEF_RETRY ) != 0 );
        lcl_addButton( *this, BUTTON_CANCEL, ( _nStyle & WB_DEF_CANCEL ) != 0 );
	}
	else
	{
        OSL_ENSURE( WB_OK & _nStyle, "OSQLMessageBox::impl_createStandardButtons: unsupported dialog style requested!" );
		AddButton( BUTTON_OK, BUTTONID_OK, BUTTONDIALOG_DEFBUTTON | BUTTONDIALOG_FOCUSBUTTON );
	}

    if ( m_sHelpURL.getLength() )
    {
        lcl_addButton( *this, BUTTON_HELP, false );

        rtl::OUString aTmp;
        INetURLObject aHID( m_sHelpURL );
        if ( aHID.GetProtocol() == INET_PROT_HID )
	  	    aTmp = aHID.GetURLPath();
        else
            aTmp = m_sHelpURL;

        SetHelpId( rtl::OUStringToOString( aTmp, RTL_TEXTENCODING_UTF8 ) );
    }
}

//------------------------------------------------------------------------------
void OSQLMessageBox::impl_addDetailsButton()
{
    size_t nFirstPageVisible = m_aMessage.IsVisible() ? 2 : 1;

    bool bMoreDetailsAvailable = m_pImpl->aDisplayInfo.size() > nFirstPageVisible;
    if ( !bMoreDetailsAvailable )
    {
        // even if the text fits into what we can display, we might need to details button
        // if there is more non-trivial information in the errors than the mere messages
        for (   ExceptionDisplayChain::const_iterator error = m_pImpl->aDisplayInfo.begin();
                error != m_pImpl->aDisplayInfo.end();
                ++error
            )
        {
            if ( lcl_hasDetails( *error ) )
            {
                bMoreDetailsAvailable = true;
                break;
            }
        }
    }

    if ( bMoreDetailsAvailable )
	{
        AddButton( BUTTON_MORE, BUTTONID_MORE, 0 );
        PushButton* pButton = GetPushButton( BUTTONID_MORE );
        OSL_ENSURE( pButton, "OSQLMessageBox::impl_addDetailsButton: just added this button, why isn't it there?" );
		pButton->SetClickHdl( LINK( this, OSQLMessageBox, ButtonClickHdl ) );
		pButton->SetUniqueId( UID_SQLERROR_BUTTONMORE );
	}
}

//------------------------------------------------------------------------------
void OSQLMessageBox::Construct( WinBits _nStyle, MessageType _eImage )
{
	// Changed as per BugID 79541 Branding/Configuration
    String sDialogTitle( lcl_getProductName() );
    SetText( sDialogTitle.AppendAscii( " Base" ) );

    // position and size the controls and the dialog, depending on whether we have one or two texts to display
    impl_positionControls();

    // init the image
    MessageType eType( _eImage );
    if ( eType == AUTO )
    {
        switch ( m_pImpl->aDisplayInfo[0].eType )
        {
        case SQLExceptionInfo::SQL_EXCEPTION: eType = Error;    break;
        case SQLExceptionInfo::SQL_WARNING:   eType = Warning;  break;
        case SQLExceptionInfo::SQL_CONTEXT:   eType = Info;     break;
        default: OSL_ENSURE( false, "OSQLMessageBox::Construct: invalid type!" );
        }
    }
    impl_initImage( eType );

	// create buttons
    impl_createStandardButtons( _nStyle );
    impl_addDetailsButton();
}

//------------------------------------------------------------------------------
OSQLMessageBox::OSQLMessageBox(Window* _pParent, const SQLExceptionInfo& _rException, WinBits _nStyle, const ::rtl::OUString& _rHelpURL )
    :ButtonDialog( _pParent, WB_HORZ | WB_STDDIALOG )
    ,m_aInfoImage( this )
    ,m_aTitle( this, WB_WORDBREAK | WB_LEFT )
    ,m_aMessage( this, WB_WORDBREAK | WB_LEFT )
    ,m_sHelpURL( _rHelpURL )
    ,m_pImpl( new SQLMessageBox_Impl( _rException ) )
{
	Construct( _nStyle, AUTO );
}

//------------------------------------------------------------------------------
OSQLMessageBox::OSQLMessageBox( Window* _pParent, const UniString& _rTitle, const UniString& _rMessage, WinBits _nStyle, MessageType _eType, const ::dbtools::SQLExceptionInfo* _pAdditionalErrorInfo )
    :ButtonDialog( _pParent, WB_HORZ | WB_STDDIALOG )
    ,m_aInfoImage( this )
    ,m_aTitle( this, WB_WORDBREAK | WB_LEFT )
    ,m_aMessage( this, WB_WORDBREAK | WB_LEFT )
{
    SQLContext aError;
    aError.Message = _rTitle;
    aError.Details = _rMessage;
    if ( _pAdditionalErrorInfo )
        aError.NextException = _pAdditionalErrorInfo->get();

    m_pImpl.reset( new SQLMessageBox_Impl( SQLExceptionInfo( aError ) ) );

    Construct( _nStyle, _eType );
}

//--------------------------------------------------------------------------
OSQLMessageBox::~OSQLMessageBox()
{
}

//--------------------------------------------------------------------------
IMPL_LINK( OSQLMessageBox, ButtonClickHdl, Button *, /*pButton*/ )
{
	OExceptionChainDialog aDlg( this, m_pImpl->aDisplayInfo );
	aDlg.Execute();
	return 0;
}

//==================================================================
// OSQLWarningBox
//==================================================================
OSQLWarningBox::OSQLWarningBox( Window* _pParent, const UniString& _rMessage, WinBits _nStyle,
    const ::dbtools::SQLExceptionInfo* _pAdditionalErrorInfo )
    :OSQLMessageBox( _pParent, String( ModuleRes( STR_STAT_WARNING ) ), _rMessage, _nStyle, OSQLMessageBox::Warning, _pAdditionalErrorInfo )
{
}

//.........................................................................
}	// namespace dbaui
//.........................................................................

