%define name    conglomerate
%define version 0.5.5
%define release 1
%define prefix  /usr
 
Summary:        Extensible XML Editor
Name:           %{name}
Version:        %{version}
Release:        %{release}
Copyright:      GPL
Group:          Applications/Editors
URL:            http://www.conglomerate.org/
Vendor:         The Conglomerate Team <developers@conglomerate.org>
Source:         %{name}-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}
 
%description
Conglomerate is a complete system for working with documents. It lets
the user
create, revise, archive, search, convert and publish information in
several
media, using a single source document.The system consists of a graphical
frontend for all user operations, and a server/database which performs
storage, searching, revision control, transformation and publishing.
 
%prep
%setup -q
 
%build
export CXXFLAGS=3D"${RPM_OPT_FLAGS}";
./configure --prefix=3D%{prefix}
 
make
 
%install
[ -d ${RPM_BUILD_ROOT} ] && rm -rf ${RPM_BUILD_ROOT};
mkdir -p ${RPM_BUILD_ROOT}
make prefix=3D${RPM_BUILD_ROOT}%{prefix} install-strip
 
%clean
[ -d ${RPM_BUILD_ROOT} ] && rm -rf ${RPM_BUILD_ROOT};
 
%files
%defattr(-,root,root)
%doc AUTHORS BUGS COPYING ChangeLog INSTALL NEWS README TODO
 
%{prefix}/bin/conglomerate
%{prefix}/share/*
%{prefix}/var/*
 
%changelog
* Sat Jun 06 2003 Michael Rasmussen <mir@datanom.net> (0.5.3 release)
- First version of this spec file
 
 
 

