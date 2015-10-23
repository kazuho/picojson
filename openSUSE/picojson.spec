#
# spec file for package picojson
#
# Copyright (c) 2014 Kapil Arya <kapil@mesosphere.io>
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

Name:           picojson
Version:        1.3.0
Release:        0
Summary:        A header-file-only, JSON parser / serializer in C++
License:        BSD-2-Clause
URL:            https://github.com/kazuho/picojson
Group:		Development/Libraries/C++

#Git-Clone:	git://github.com/kazuho/picojson
Source: 	%{name}-%{version}.tar.gz
BuildRequires:  gcc-c++

%description
PicoJSON is a tiny JSON parser / serializer for C++. It is implemented as
header-only, has no external dependencies, is STL-friendly (using std::vector
and std::map only), and provides both streaming (event-based) and a pull
interface.
This package provides the picojson header file(s).

%package devel
Summary:        Header files for picojson development
Group:          Development/Libraries/C and C++
Provides:       %{name}-static = %{version}-%{release}

%description devel
PicoJSON is a tiny JSON parser / serializer for C++. It is implemented as
header-only, has no external dependencies, is STL-friendly (using std::vector
and std::map only), and provides both streaming (event-based) and a pull
interface.
This package provides the picojson header file(s).

%prep
%setup -q

%build

%check
make test

%install
make install includedir=%{_includedir} DESTDIR=%{buildroot} %{?_smp_mflags}

%files devel
%defattr(-,root,root)
%{_includedir}/picojson.h
%doc LICENSE README.mkdn examples

%changelog
