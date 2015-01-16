#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
static void printHelp(void) {
    printf("Extract:\nAccession <tab> GI <tab> ORGANISM\nfrom a GenBank flatfile.\n");
}
static void errmessage1(const char * fmt, const char * arg) { fprintf(stderr, fmt, arg); }
/*
 static void errmessage1d(const char * fmt, int arg) { fprintf(stderr, fmt, arg); }
*/
static void errmessage1d1s(const char * fmt, int arg, const char * arg2) { fprintf(stderr, fmt, arg, arg2); }
#define MAX_LINE_LEN 200
char gLineBuff[MAX_LINE_LEN];
inline int startswith(const char * prefix, const unsigned int prefLen, const char * test, const unsigned int lineLen) {
    if (lineLen < prefLen) {
        return 0;
    }
    return (strncmp(prefix, test, prefLen) == 0 ? 1 : 0);
}
char * getNextLine(FILE *f, unsigned int *lineLen, int *atEnd) {
    if (fgets(gLineBuff, MAX_LINE_LEN, f) == 0L) {
        *atEnd = 1;
        return 0L;
    }
    *lineLen = strlen(gLineBuff);
    if (*lineLen >= MAX_LINE_LEN - 1) {
        errmessage1("need to increase %s\n", "MAX_LINE_LEN");
        return 0L; /* need to increase MAX_LINE_LEN */
    }
    return gLineBuff;
}
inline unsigned int skipRecord(FILE *f, int *atEnd) {
    unsigned int nErrors = 0;
    char * buffAlias;
    unsigned int lineLen;
    for (;;) {
        buffAlias = getNextLine(f, &lineLen, atEnd); if (buffAlias == 0L) {if (*atEnd == 0) {nErrors += 1;} return nErrors;}
        if (startswith("//", 2, buffAlias, lineLen)) {
            break;
        }
    }
    return nErrors;
}
inline unsigned int skipToGraphOrNull(const char * source) {
    unsigned int i = 0;
    while (source[i] != '\0') {
        if (isgraph(source[i])) {
            break;
        }
        i++;
    }
    return i;
}
inline unsigned int copyGraphToEOL(const char * source, char * dest) {
    unsigned int i = skipToGraphOrNull(source);
    unsigned int j = 0;
    if (source[i] != '\0') {
        dest[j++] = source[i++];
        while (source[i] != '\0') {
            if (source[i] == '\n') {
                break;
            }
            dest[j++] = source[i++];
        }
    }
    dest[j] = '\0';
    return i;
}

inline unsigned int copyWord(const char * source, char * dest) {
    unsigned int i = skipToGraphOrNull(source);
    unsigned int j = 0;
    if (source[i] != '\0') {
        dest[j++] = source[i++];
        while (source[i] != '\0') {
            if (0 == isgraph(source[i])) {
                break;
            }
            dest[j++] = source[i++];
        }
    }
    dest[j] = '\0';
    return i;
}
#define VERSION_LEN 7
#define FEATURES_LEN 8
#define SOURCE_LEN 6
#define SOURCE_FEATURE_LEN 11
#define TAXON_PREF_LEN 37
inline void slideLeft(char *line, unsigned offset) {
    unsigned int i;
    for (i = 0;; ++i) {
        line[i] = line[i + offset];
        if (line[i + offset] == '\0') {
            return;
        }
    }
}
inline int parseTaxonID(const char *line, char * taxon) {
    const unsigned int offset = TAXON_PREF_LEN;
    unsigned i = 0;
    while (line[offset + i] != '\0' && line[offset + i] != '"') {
        taxon[i] = line[offset + i];
        ++i;
    }
    taxon[i] = '\0';
    return i > 0 ? 1 : 0;
}



inline int parseSource(const char *line, char * source) {
    copyGraphToEOL(line + SOURCE_LEN + 1, source);
    if (strlen(source) < 2) {
        errmessage1("Could not parse an SOURCE out of \"%s\"\n", line);
        return 0;
    }
    return 1;
}

inline int parseAccessionGI(const char *line, char * accession, char * gi) {
    unsigned int pos, p2;
    pos = VERSION_LEN + 1 + copyWord(line + VERSION_LEN + 1, accession);
    if (strlen(accession) < 4) {
        errmessage1("Could not parse an ACCESSION out of \"%s\"\n", line);
        return 0;
    }
    p2 = copyWord(line + pos, gi);
    if (p2 < 4 || 0 == startswith("GI:", 3, gi, strlen(gi))) {
        errmessage1("GI \"%s\"\n", gi);
        errmessage1("Could not parse a GI out of \"%s\"\n", line);
        return 0;
    }
    slideLeft(gi, 3);
    return 1;
}
static unsigned int skimSourceFeatures(FILE * f, int * atEnd, unsigned int * nErrors, unsigned int * numTaxonIDs) {
    char taxonID[MAX_LINE_LEN];
    char * buffAlias;
    unsigned int lineLen;
    int hadError = 0;
    for (;;) {
        buffAlias = getNextLine(f, &lineLen, atEnd); if (buffAlias == 0L) {if (*atEnd == 0) {*nErrors += 1;} return 0;}
        if (lineLen > 0 && isgraph(buffAlias[0])) {
            break;
        }
        if (lineLen > 5 && isgraph(buffAlias[5])) {
            if (!startswith("     source", SOURCE_FEATURE_LEN, buffAlias, lineLen)) {
                break;
            }
        }
        if (startswith("                     /db_xref=\"taxon:", TAXON_PREF_LEN, buffAlias, lineLen)) {
            if (parseTaxonID(buffAlias, taxonID)) {
                *numTaxonIDs += 1;
                printf("\t%s", taxonID);
            } else {
                *nErrors += 1;
                hadError = 1;
                errmessage1("Could not parse a taxon ID out of \"%s\"\n", buffAlias);
            }
        }
    }
    return (hadError ? 0 : 1);
}



static unsigned int skimFeatures(FILE * f, int * atEnd, unsigned int * nErrors, unsigned int * numTaxonIDs) {
    char * buffAlias;
    unsigned int lineLen;
    for (;;) {
        buffAlias = getNextLine(f, &lineLen, atEnd); if (buffAlias == 0L) {if (*atEnd == 0) {*nErrors += 1;} return 0;}
        if (lineLen > 0 && isgraph(buffAlias[0])) {
            break;
        }
        if (startswith("     source", SOURCE_FEATURE_LEN, buffAlias, lineLen)) {
            if (!skimSourceFeatures(f, atEnd, nErrors, numTaxonIDs)) {
                return 0;
            }
        }
    }
    return 1;
}

static unsigned int readrecord(FILE *f, int *atEnd) {
    unsigned int nErrors = 0;
    char * buffAlias;
    unsigned int lineLen;
    int versionFound = 0;
    int sourceFound = 0;
    int featureFound = 0;
    char accessionField[MAX_LINE_LEN];
    char giField[MAX_LINE_LEN];
    char sourceField[MAX_LINE_LEN];
    unsigned numTaxonIDs = 0;
    for (;;) {
        buffAlias = getNextLine(f, &lineLen, atEnd); if (buffAlias == 0L) {if (*atEnd == 0) {nErrors += 1;} return nErrors;}
        if (startswith("//", 2, buffAlias, lineLen)) {
            if (versionFound == 0) {
                errmessage1("EOR with no %s.\n", "VERSION");
                nErrors += 1;
            } else if (sourceFound == 0) {
                errmessage1("EOR with no SOURCE. ACCESSION = %s\n", accessionField);
                nErrors += 1;
            } else if (numTaxonIDs != 1) {
                errmessage1d1s("EOR with %d taxon IDs found ACCESSION = %s.\n", numTaxonIDs, accessionField);
                nErrors += 1;
            }
            break;
        } else if (startswith("VERSION", VERSION_LEN, buffAlias, lineLen)) {
            if (versionFound == 1) {
                errmessage1("record with multiple VERSION entries. The first had ACCESSION=%s\n", accessionField);
            }
            if (parseAccessionGI(buffAlias, accessionField, giField) == 0) {
                nErrors += 1 + skipRecord(f, atEnd);
                break;
            }
            versionFound = 1;
        } else if (startswith("SOURCE", SOURCE_LEN, buffAlias, lineLen)) {
            if (versionFound == 0) {
                errmessage1("FEATURES with no %s.\n", "VERSION");
                nErrors += 1 + skipRecord(f, atEnd);
                break;
            } else if (parseSource(buffAlias, sourceField) == 0) {
                nErrors += 1 + skipRecord(f, atEnd);
                break;
            }
            sourceFound = 1;
            printf("%s\t%s\t%s", accessionField, giField, sourceField);
        } else if (startswith("FEATURES", FEATURES_LEN, buffAlias, lineLen)) {
            if (versionFound == 0) {
                errmessage1("FEATURES with no %s.\n", "VERSION");
                nErrors += 1 + skipRecord(f, atEnd);
                break;
            } else if (sourceFound == 0) {
                errmessage1("FEATURES with no SOURCE. ACCESSION = %s\n", accessionField);
                nErrors += 1 + skipRecord(f, atEnd);
                break;
            } else if (featureFound != 0) {
                errmessage1("multiple FEATURES found. ACCESSION = %s\n", accessionField);
                nErrors += 1 + skipRecord(f, atEnd);
                break;
            }
            featureFound = 1;
            if (!skimFeatures(f, atEnd, &nErrors, &numTaxonIDs)) {
                break;
            }
        }
    }
    if (sourceFound) {
        printf("\n");
    }
    return nErrors;
}
static unsigned int readgb(FILE *f) {
    unsigned int nErrors = 0;
    int atEnd = 0;
    while (atEnd == 0) {
        nErrors += readrecord(f, &atEnd);
        if (atEnd == 0) {
            /*printf("record\n");*/
        }
    }
    return nErrors;
}
int main(int argc, char * argv[]) {
    char * fn;
    FILE * filep = 0L;
    if (argc != 2) {
        printHelp();
        return 1;
    }
    fn = argv[1];
    filep = fopen(fn, "r");
    if (filep == 0L) {
        errmessage1("Could not open \"%s\"\n", fn);
        return 1;
    }
    return readgb(filep);
}
