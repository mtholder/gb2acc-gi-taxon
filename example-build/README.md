# building an Accession, GI -> taxon ID mapping
You proabably don't want to run this. It takes days and >50GB of storage.
It might be helpful

A complete invocation example would be:

    $ sh initial-download.sh
    $ mkdir daily
    $ echo daily > daily-subdir.txt
    $ export PATH=$PWD:$PATH # we need create-ago*.sh on our PATH
    $ rebuild-acc-gi-2-taxon.sh
    $ python acc-gi-taxon-condenser.py COMBINED-ago.tsv acc-gi-taxon.pickle

If this works for you, then you can just execute the last two steps to 
    update the mapping.
Note that only some DIVs of GenBank are downloaded, but the daily incremental
    updates are not filtered to be limited to those DIVs.
So the mapping will only be complete for the subset of the DIVs (but will probably
    have some entries for all DIVs if you are running this after any daily incremental
    updates have been posted).

The first 4 steps download the flatfiles from GenBank and compress
    the (non-daily) files with xz. They also create a list of
    the subdirectories downloaded (in release-subdirs.txt) and
    record the directory used for the daily incremental updates
    in daily-subdir.txt
These are the prep steps for the rebuild-acc-gi-2-taxon.sh script
    which will create a (very large) COMBINED-ago.tsv file.
That script uses `create-ago-gz.sh` and `create-ago-xz.sh` and
    the `gb2acc-gi-taxon` tool to create tsv files in which the columns are:
    
    1. ACCESSION
    2. GI
    3. source (taxon name)
    4. the NCBI taxon IDs from the FEATURES table.

The last step condenses that table into a (still very large) python
    pickle of a dict mapping the accession (without version # suffix) and
    the GI to the taxon IDS.
The dict disregards sequences that have more than one taxon ID in their FEATURES
    table.
The pickled from of the dict is the last arg of the last line

# intitial-download.sh
This is very slow. It
    1. uses rsync to download the files for selected divisions;
    2. compresses each file with xz max compression to save space;
    3. adds the directory name to a release-subdirs.txt file

The divisions included are
       bct con inv mam pln pri rod tsa vrt
This omits:
     vrl viral
     phg phage
     syn synthetic
     una unannotated
     est Expressed seq tags
     pat patented
     sts Sequence tagged sites
     gss genomic survey seqs
     htg high throughput genomic seq
     htc high throughput cDNA seq
     env environmental

See ftp://ftp.ncbi.nih.gov/genbank/gbrel.txt

# rebuild-acc-gi-2-taxon.sh
This will go through all of the DIVs downloaded from the release
and make sure that there each file has been processed to extract the
taxonomic mapping. 
This step is done with `create-ago-xz.sh` for the genebank release and
the `-gz.sh` for the daily snapshot.

For the daily snapshot, an rsync command is also issued to refresh the
directory.

