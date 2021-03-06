#**************************************************************
#  
#  Licensed to the Apache Software Foundation (ASF) under one
#  or more contributor license agreements.  See the NOTICE file
#  distributed with this work for additional information
#  regarding copyright ownership.  The ASF licenses this file
#  to you under the Apache License, Version 2.0 (the
#  "License"); you may not use this file except in compliance
#  with the License.  You may obtain a copy of the License at
#  
#    http://www.apache.org/licenses/LICENSE-2.0
#  
#  Unless required by applicable law or agreed to in writing,
#  software distributed under the License is distributed on an
#  "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#  KIND, either express or implied.  See the License for the
#  specific language governing permissions and limitations
#  under the License.
#  
#**************************************************************



#i20156 - new file for xmlsecurity module

PRJ=..$/..$/..$/..

PRJNAME=offapi

TARGET=xsec-security
PACKAGE=com$/sun$/star$/security

# --- Settings -----------------------------------------------------
.INCLUDE :  $(PRJ)$/util$/makefile.pmk

# ------------------------------------------------------------------------

IDLFILES=\
    CertificateException.idl            \
    CryptographyException.idl           \
    DocumentDigitalSignatures.idl       \
    DocumentSignatureInformation.idl   \
    EncryptionException.idl             \
    KeyException.idl                    \
    NoPasswordException.idl             \
    SecurityInfrastructureException.idl \
    SignatureException.idl              \
    XCertificate.idl                    \
    CertificateCharacters.idl           \
    CertificateValidity.idl             \
    XCertificateExtension.idl           \
    XDocumentDigitalSignatures.idl	\
    KeyUsage.idl			\
    XCertificateContainer.idl		\
    XSerialNumberAdapter.idl		\
    SerialNumberAdapter.idl		\
    CertificateContainer.idl \
    CertificateContainerStatus.idl \
    ExtAltNameType.idl \
    XSanExtension.idl \
    CertAltNameEntry.idl
    
    
    

# ------------------------------------------------------------------

.INCLUDE :  target.mk
.INCLUDE :  $(PRJ)$/util$/target.pmk
