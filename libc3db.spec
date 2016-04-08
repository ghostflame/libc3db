Name:		libc3db
Version:	0.1.3
Release:	1%{?dist}
Summary:	A library for managing C3DB format time-series databases.

Group:		Applications/Internet
License:	Apache License 2.0 (https://www.apache.org/licenses/LICENSE-2.0.txt)
URL:		https://github.com/ghostflame/libc3db
Source:		https://github.com/ghostflame/libc3db/archive/%{version}.tar.gz

BuildRequires: gcc

%description
This library manages C3DB format files, which are time-series databases of
similar design to RRD and Graphite's whisper format.  They are intended for
use in metric storage and are used by Coal.  The library is written in C and
should be thread-safe.

%prep
%setup -q

%build
make %{?_smp_mflags}

%install
DESTDIR=%{buildroot} \
LIBDIR=%{buildroot}%{_libdir} \
BINDIR=%{buildroot}%{_bindir} \
MANDIR=%{buildroot}%{_mandir} \
INCDIR=%{buildroot}%{_includedir} \
DOCDIR=%{buildroot}%{_docdir}/libc3db \
VERSION=%{version} \
make install


%post -p /sbin/ldconfig


%postun -p /sbin/ldconfig


%files
%{_libdir}/libc3db.so
%{_libdir}/libc3db.a
%{_libdir}/libc3db.so.%{version}
%{_includedir}/c3db.h
%{_bindir}/c3db_create
%{_bindir}/c3db_dump
%{_bindir}/c3db_query
%{_bindir}/c3db_update
%{_mandir}/man3/libc3db.3.gz
%{_docdir}/libc3db/

%changelog

