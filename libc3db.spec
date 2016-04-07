Name:		libc3db
Version:	0.3.0
Release:	1%{?dist}
Summary:	A library for managing C3DB format time-series databases.

Group:		Applications/Internet
License:	Apache License 2.0 (https://www.apache.org/licenses/LICENSE-2.0.txt)
URL:		https://github.com/ghostflame/libc3db
Source:		https://github.com/ghostflame/libc3db/archive/%{version}.tar.gz

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
DESTDIR=%{buildroot} make install

%files
%doc
/usr/lib/libc3db.so
/usr/lib/libc3db.so.%{version}
/usr/lib/libc3db.a
/usr/include/c3db.h
/usr/bin/c3db_create
/usr/bin/c3db_dump
/usr/bin/c3db_query
/usr/bin/c3db_update
/usr/share/doc/libc3db/
/usr/share/man/man3/libc3db.3.gz

%changelog

