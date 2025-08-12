#pragma once

#ifndef _COMPILETIMESTAMP_H_
#define _COMPILETIMESTAMP_H_

#ifdef COMPILETIMESTAMP_USE_64BIT

#define _SEC_PER_MIN  60ULL
#define _SEC_PER_HOUR 3600ULL
#define _SEC_PER_DAY  86400ULL
#define _SEC_PER_YEAR (_SEC_PER_DAY * 365ULL)

#define _CONV_STR2DEC_1(STRING, i) (STRING[i] > '0' ? STRING[i] - '0' : 0ULL)
#define _CONV_STR2DEC_2(STRING, i) (_CONV_STR2DEC_1(STRING, i) * 10ULL + STRING[i + 1ULL] - '0')
#define _CONV_STR2DEC_3(STRING, i) (_CONV_STR2DEC_2(STRING, i) * 10ULL + STRING[i + 2ULL] - '0')
#define _CONV_STR2DEC_4(STRING, i) (_CONV_STR2DEC_3(STRING, i) * 10ULL + STRING[i + 3ULL] - '0')

#define _GET_MONTH(STRING, i) (((STRING[i] == 'J') && (STRING[i + 1ULL] == 'a') && (STRING[i + 2ULL] == 'n')) ?  1ULL :       \
							   ((STRING[i] == 'F') && (STRING[i + 1ULL] == 'e') && (STRING[i + 2ULL] == 'b')) ?  2ULL :       \
							   ((STRING[i] == 'M') && (STRING[i + 1ULL] == 'a') && (STRING[i + 2ULL] == 'r')) ?  3ULL :       \
							   ((STRING[i] == 'A') && (STRING[i + 1ULL] == 'p') && (STRING[i + 2ULL] == 'r')) ?  4ULL :       \
							   ((STRING[i] == 'M') && (STRING[i + 1ULL] == 'a') && (STRING[i + 2ULL] == 'y')) ?  5ULL :       \
							   ((STRING[i] == 'J') && (STRING[i + 1ULL] == 'u') && (STRING[i + 2ULL] == 'n')) ?  6ULL :       \
							   ((STRING[i] == 'J') && (STRING[i + 1ULL] == 'u') && (STRING[i + 2ULL] == 'l')) ?  7ULL :       \
							   ((STRING[i] == 'A') && (STRING[i + 1ULL] == 'u') && (STRING[i + 2ULL] == 'g')) ?  8ULL :       \
							   ((STRING[i] == 'S') && (STRING[i + 1ULL] == 'e') && (STRING[i + 2ULL] == 'p')) ?  9ULL :       \
							   ((STRING[i] == 'O') && (STRING[i + 1ULL] == 'c') && (STRING[i + 2ULL] == 't')) ? 10ULL :       \
							   ((STRING[i] == 'N') && (STRING[i + 1ULL] == 'o') && (STRING[i + 2ULL] == 'v')) ? 11ULL :       \
							   ((STRING[i] == 'D') && (STRING[i + 1ULL] == 'e') && (STRING[i + 2ULL] == 'c')) ? 12ULL : 0ULL)

#define __TIME_SECONDS__ _CONV_STR2DEC_2(__TIME__, 6ULL)
#define __TIME_MINUTES__ _CONV_STR2DEC_2(__TIME__, 3ULL)
#define __TIME_HOURS__   _CONV_STR2DEC_2(__TIME__, 0ULL)
#define __TIME_DAYS__    _CONV_STR2DEC_2(__DATE__, 4ULL)
#define __TIME_MONTH__        _GET_MONTH(__DATE__, 0ULL)
#define __TIME_YEARS__   _CONV_STR2DEC_4(__DATE__, 7ULL)

#define _UNIX_TIMESTAMP_FDAY(YEAR)             \
	(((YEAR) % 400ULL) == 0ULL ? 29ULL :       \
		(((YEAR) % 100ULL) == 0ULL ? 28ULL :   \
			(((YEAR) % 4ULL) == 0ULL ? 29ULL : \
				28ULL)))

#define _UNIX_TIMESTAMP_YDAY(YEAR, MONTH, DAY)                                 \
	(                                                                          \
		/* January */     DAY                                                  \
		/* February */  + (MONTH >=  2ULL ? 31ULL : 0ULL)                      \
		/* March */     + (MONTH >=  3ULL ? _UNIX_TIMESTAMP_FDAY(YEAR) : 0ULL) \
		/* April */     + (MONTH >=  4ULL ? 31ULL : 0ULL)                      \
		/* May */       + (MONTH >=  5ULL ? 30ULL : 0ULL)                      \
		/* June */      + (MONTH >=  6ULL ? 31ULL : 0ULL)                      \
		/* July */      + (MONTH >=  7ULL ? 30ULL : 0ULL)                      \
		/* August */    + (MONTH >=  8ULL ? 31ULL : 0ULL)                      \
		/* September */ + (MONTH >=  9ULL ? 31ULL : 0ULL)                      \
		/* October */   + (MONTH >= 10ULL ? 30ULL : 0ULL)                      \
		/* November */  + (MONTH >= 11ULL ? 31ULL : 0ULL)                      \
		/* December */  + (MONTH >= 12ULL ? 30ULL : 0ULL)                      \
	)

#define _UNIX_TIMESTAMP(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND)                                   \
	( /* TIME */    SECOND                                                                        \
				  + MINUTE * _SEC_PER_MIN                                                         \
				  + HOUR * _SEC_PER_HOUR                                                          \
	+ /* YEAR DAY (MONTH + DAY) */ (_UNIX_TIMESTAMP_YDAY(YEAR, MONTH, DAY) - 1ULL) * _SEC_PER_DAY \
	+ /* YEAR */   (YEAR - 1970ULL) * _SEC_PER_YEAR                                               \
				+ ((YEAR - 1969ULL) / 4ULL) * _SEC_PER_DAY                                        \
				- ((YEAR - 1901ULL) / 100ULL) * _SEC_PER_DAY                                      \
				+ ((YEAR - 1601ULL) / 400ULL) * _SEC_PER_DAY                                      \
	)

#define __UNIX_TIMESTAMP_SECONDS__ (_UNIX_TIMESTAMP(__TIME_YEARS__, __TIME_MONTH__, __TIME_DAYS__, __TIME_HOURS__, __TIME_MINUTES__, __TIME_SECONDS__))
#define __UNIX_TIMESTAMP_MINUTES__ (_UNIX_TIMESTAMP(__TIME_YEARS__, __TIME_MONTH__, __TIME_DAYS__, __TIME_HOURS__, __TIME_MINUTES__,             0ULL))
#define __UNIX_TIMESTAMP_HOURS__   (_UNIX_TIMESTAMP(__TIME_YEARS__, __TIME_MONTH__, __TIME_DAYS__, __TIME_HOURS__,             0ULL,             0ULL))
#define __UNIX_TIMESTAMP_DAYS__    (_UNIX_TIMESTAMP(__TIME_YEARS__, __TIME_MONTH__, __TIME_DAYS__,           0ULL,             0ULL,             0ULL))
#define __UNIX_TIMESTAMP_MONTHS__  (_UNIX_TIMESTAMP(__TIME_YEARS__, __TIME_MONTH__,          1ULL,           0ULL,             0ULL,             0ULL))
#define __UNIX_TIMESTAMP_YEARS__   (_UNIX_TIMESTAMP(__TIME_YEARS__,           1ULL,          1ULL,           0ULL,             0ULL,             0ULL))

#else

#define _SEC_PER_MIN  60UL
#define _SEC_PER_HOUR 3600UL
#define _SEC_PER_DAY  86400UL
#define _SEC_PER_YEAR (_SEC_PER_DAY * 365UL)

#define _CONV_STR2DEC_1(STRING, i) (STRING[i] > '0' ? STRING[i] - '0' : 0UL)
#define _CONV_STR2DEC_2(STRING, i) (_CONV_STR2DEC_1(STRING, i) * 10UL + STRING[i + 1UL] - '0')
#define _CONV_STR2DEC_3(STRING, i) (_CONV_STR2DEC_2(STRING, i) * 10UL + STRING[i + 2UL] - '0')
#define _CONV_STR2DEC_4(STRING, i) (_CONV_STR2DEC_3(STRING, i) * 10UL + STRING[i + 3UL] - '0')

#define _GET_MONTH(STRING, i) (((STRING[i] == 'J') && (STRING[i + 1UL] == 'a') && (STRING[i + 2UL] == 'n')) ?  1UL :      \
							   ((STRING[i] == 'F') && (STRING[i + 1UL] == 'e') && (STRING[i + 2UL] == 'b')) ?  2UL :      \
							   ((STRING[i] == 'M') && (STRING[i + 1UL] == 'a') && (STRING[i + 2UL] == 'r')) ?  3UL :      \
							   ((STRING[i] == 'A') && (STRING[i + 1UL] == 'p') && (STRING[i + 2UL] == 'r')) ?  4UL :      \
							   ((STRING[i] == 'M') && (STRING[i + 1UL] == 'a') && (STRING[i + 2UL] == 'y')) ?  5UL :      \
							   ((STRING[i] == 'J') && (STRING[i + 1UL] == 'u') && (STRING[i + 2UL] == 'n')) ?  6UL :      \
							   ((STRING[i] == 'J') && (STRING[i + 1UL] == 'u') && (STRING[i + 2UL] == 'l')) ?  7UL :      \
							   ((STRING[i] == 'A') && (STRING[i + 1UL] == 'u') && (STRING[i + 2UL] == 'g')) ?  8UL :      \
							   ((STRING[i] == 'S') && (STRING[i + 1UL] == 'e') && (STRING[i + 2UL] == 'p')) ?  9UL :      \
							   ((STRING[i] == 'O') && (STRING[i + 1UL] == 'c') && (STRING[i + 2UL] == 't')) ? 10UL :      \
							   ((STRING[i] == 'N') && (STRING[i + 1UL] == 'o') && (STRING[i + 2UL] == 'v')) ? 11UL :      \
							   ((STRING[i] == 'D') && (STRING[i + 1UL] == 'e') && (STRING[i + 2UL] == 'c')) ? 12UL : 0UL)

#define __TIME_SECONDS__ _CONV_STR2DEC_2(__TIME__, 6UL)
#define __TIME_MINUTES__ _CONV_STR2DEC_2(__TIME__, 3UL)
#define __TIME_HOURS__   _CONV_STR2DEC_2(__TIME__, 0UL)
#define __TIME_DAYS__    _CONV_STR2DEC_2(__DATE__, 4UL)
#define __TIME_MONTH__        _GET_MONTH(__DATE__, 0UL)
#define __TIME_YEARS__   _CONV_STR2DEC_4(__DATE__, 7UL)

#define _UNIX_TIMESTAMP_FDAY(YEAR)          \
	(((YEAR) % 400UL) == 0UL ? 29UL :       \
		(((YEAR) % 100UL) == 0UL ? 28UL :   \
			(((YEAR) % 4UL) == 0UL ? 29UL : \
				28UL)))

#define _UNIX_TIMESTAMP_YDAY(YEAR, MONTH, DAY)                               \
	(                                                                        \
		/* January */     DAY                                                \
		/* February */  + (MONTH >=  2UL ? 31UL : 0UL)                       \
		/* March */     + (MONTH >=  3UL ? _UNIX_TIMESTAMP_FDAY(YEAR) : 0UL) \
		/* April */     + (MONTH >=  4UL ? 31UL : 0UL)                       \
		/* May */       + (MONTH >=  5UL ? 30UL : 0UL)                       \
		/* June */      + (MONTH >=  6UL ? 31UL : 0UL)                       \
		/* July */      + (MONTH >=  7UL ? 30UL : 0UL)                       \
		/* August */    + (MONTH >=  8UL ? 31UL : 0UL)                       \
		/* September */ + (MONTH >=  9UL ? 31UL : 0UL)                       \
		/* October */   + (MONTH >= 10UL ? 30UL : 0UL)                       \
		/* November */  + (MONTH >= 11UL ? 31UL : 0UL)                       \
		/* December */  + (MONTH >= 12UL ? 30UL : 0UL)                       \
	)

#define _UNIX_TIMESTAMP(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND)                                  \
	( /* TIME */    SECOND                                                                       \
				  + MINUTE * _SEC_PER_MIN                                                        \
				  + HOUR * _SEC_PER_HOUR                                                         \
	+ /* YEAR DAY (MONTH + DAY) */ (_UNIX_TIMESTAMP_YDAY(YEAR, MONTH, DAY) - 1UL) * _SEC_PER_DAY \
	+ /* YEAR */   (YEAR - 1970UL) * _SEC_PER_YEAR                                               \
				+ ((YEAR - 1969UL) / 4UL) * _SEC_PER_DAY                                         \
				- ((YEAR - 1901UL) / 100UL) * _SEC_PER_DAY                                       \
				+ ((YEAR - 1601UL) / 400UL) * _SEC_PER_DAY                                       \
	)

#define __UNIX_TIMESTAMP_SECONDS__ (_UNIX_TIMESTAMP(__TIME_YEARS__, __TIME_MONTH__, __TIME_DAYS__, __TIME_HOURS__, __TIME_MINUTES__, __TIME_SECONDS__))
#define __UNIX_TIMESTAMP_MINUTES__ (_UNIX_TIMESTAMP(__TIME_YEARS__, __TIME_MONTH__, __TIME_DAYS__, __TIME_HOURS__, __TIME_MINUTES__,              0UL))
#define __UNIX_TIMESTAMP_HOURS__   (_UNIX_TIMESTAMP(__TIME_YEARS__, __TIME_MONTH__, __TIME_DAYS__, __TIME_HOURS__,              0UL,              0UL))
#define __UNIX_TIMESTAMP_DAYS__    (_UNIX_TIMESTAMP(__TIME_YEARS__, __TIME_MONTH__, __TIME_DAYS__,            0UL,              0UL,              0UL))
#define __UNIX_TIMESTAMP_MONTHS__  (_UNIX_TIMESTAMP(__TIME_YEARS__, __TIME_MONTH__,            1UL,           0UL,              0UL,              0UL))
#define __UNIX_TIMESTAMP_YEARS__   (_UNIX_TIMESTAMP(__TIME_YEARS__,             1UL,           1UL,           0UL,              0UL,              0UL))

#endif

#endif // !_COMPILETIMESTAMP_H_
