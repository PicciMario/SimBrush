***********************************
* SimBrush - Smart Card Forensics *
***********************************

1) imager tool:
it is used to size a SIM/USIM card
-- it produces an XML file (e.g. filename.xml)
-- installation and setup
the imager tool requires pcsc-lite package (e.g. pcsc-lite-1.4.102) which can be installed from sources. In addition, pcsc-lite needs hal-devel package (also named libhal-dev in BackTrack5).
--installation of pcsc-lite
--installation of smart card drivers (e.g. asedriveiiie-usb-3.5)

After having installed the required library, the tool pcscd can be started:

/usr/local/sbin/pcscd -f -d

>>now the SIMBrush tool can be launched

>>./SIMBrush -o image.xml

By default, the imager tool will save the SIM/USIM contents in the specificed file (e.g. image.xml). Furthermore, a file containing the hash of the collected data will be created (es. image.md5) by guarateeing the integrity of the data.

-- example of usage

1) launch pcscd deamon tool

2) launch SIMBrush tool => ./SIMBrush -o test.xml
--select the dumping mode: 
0 => partial dump (only standard EFs will be collected)
1 => full dump (all elementary and non elementary files will be collected)


2) wrapper tool:
it is used to translate the raw image into an intellegible format
-- it receives as input a raw XML image which has been obtained by means of the sizure tool
-- it produces an XML document (e.g. filename.out.xml)
-- the following list of Perl modules is required by the wrapper tool:
XML::Simple;
Data::Dumper;
Regexp::Common qw /whitespace/;
File::Copy;
XML::XSLT;
Device::Gsm;
Encode qw/encode decode/;
GSM::Nbit;


3) reporting tool:
it is used to create a fancy report which contains different levels of details
-- packages required for installation
-- ReportLab library (www.reportlab.com) 
(python setup.py install)
-- Imaging-1.1.7 
(python setup.py install)
-- python-messaging (https://github.com/pmarti/python-messaging)
(python setup.py install)

***************
GitHub setup
Mini HowTo
***************
--generazione della chiave ssh


--per clonare un repository remoto: 
$ git clone git@github.com:PicciMario/SimBrush.git

--per valutare lo stato del repository locale
$ git status

--per effettuare una modifica al repository (dopo aver modificato il file Readme ad esempio)
$ git commit -am "modifica al file Readme"

--per sincronizzare il repository locale con quello remoto
$ git push origin master 
spiegazione: 
--git => nome applicativo
--push => operazione di upload
--origin => nome del server su cui uplodare
--master => nome del branch

--per aggiungere un file al repository (es. prova_file.txt)
$ git add prova_file.txt

--per rimuovere un file
$ git rm prova_file.txt
$ git commit -am "rimosso il file prova_file.txt"







