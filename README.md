#gb2acc-gi-taxon Parsing Accession to taxon mapping from GenBank flat files

# building

    $ ./build.sh

# usage

Takes an unzipped flat sequence file from genbank (e.g. ftp://ftp.ncbi.nih.gov/genbank/gbbct1.seq.gz )


# motivation

I had no luck with the GBParsy based parser at https://gist.github.com/mtholder/035eed5d9ed3e4ff6816

It was refusing to parse many GenBank files.

So this is very simple parser that just tries to pull out the 
    ACCESSION with version, 
    GI,
    SOURCE (organism name), and
    taxonomic ID from FEATURES/source/db_xref="taxon:#####"

these values are printed out to standard out with tab delimiters

Warnings/errors show up in standard error.

Currently uses very primitive string matching, but tries to emit lots of warnings 

# License
BSD
# Author
Mark T. Holder

