#! /usr/bin/perl

use XML::Simple;
use Data::Dumper;
use Regexp::Common qw /whitespace/;
use File::Copy;
use XML::XSLT;
#no warnings "all";

my %act = (
           '1' =>"Activated",
           '0' =>"Not activated"
          );

my %mnc = (
        '412F01' =>"Afghan Wireless Communication Company (GSM 900/1800)###Afghanistan",
        '412F20' =>"Roshan (GSM 900)###Afghanistan",
        '276F01' =>"Vodafone Albania (GSM 900/1800)###Albania",
        '603F01' =>"Algeria Mobile Network (GSM 900)###Algeria",
        '603F02' =>"Orascom (GSM 900/1800)###Algeria",
        '603F03' =>"Nedjma (GSM 900/1800)###Algeria",
        '213F03' =>"STA-MobilAnd (GSM 900)###Andorra",
        '631F02' =>"Unitel (GSM 900)###Angola",
        '344F03' =>"APUA (GSM 1900)###Antigua & Barbuda",
        '722F07' =>"Unifon###Argentine Republic",
        '722F34' =>"Telecom Personal (GSM 1900)###Argentine Republic",
        '283F01' =>"ArmenTel (GSM 900)###Armenia",
        '363F01' =>"Setar (GSM 900/1800/1900)###Aruba",
        '505F01' =>"Telstra (GSM 900/1800)###Australia",
        '505F02' =>"Cable + Wireless / Optus Telecom###Australia",
        '505F03' =>"Vodafone (GSM 900)###Australia",
        '232F01' =>"Mobilkom Austria (GSM 900)###Austria",
        '232F03' =>"max.mobil.Telekoms Service / T-Mobile Austria (GSM 900/1800)###Austria",
        '232F05' =>"Connect Austria One (GSM 1800)###Austria",
        '232F07' =>"*Telering (GSM 1800)###Austria",
        '232F10' =>"3 (UMTS)###Austria",
        '400F01' =>"Azercell Telekom B.M. (GSM 900)###Azerbaidjan",
        '400F02' =>"J.V. Bakcell GSM 2000 (GSM 900)###Azerbaidjan",
        '426F01' =>"Batelco###Bahrain",
        '426F02' =>"MTC-VFBH###Bahrain",
        '470F01' =>"Grameen Phone Ltd###Bangladesh",
        '470F19' =>"Sheeba Telecom / TM International###Bangladesh",
        '257F01' =>"Velcom###Belarus",
        '257F02' =>"MTS BY###Belarus",
        '206F01' =>"Belgacom Mobile Proximus###Belgium",
        '206F10' =>"Mobistar###Belgium",
        '206F20' =>"KPN Orange###Belgium",
        '218F01' =>"Cronet###Bosnia and Herzegovina",
        '218F19' =>"PTT Bosnia###Bosnia and Herzegovina",
        '218F90' =>"PE PTT BIH###Bosnia and Herzegovina",
        '652F01' =>"Mascom Wireless###Botswana",
        '724F02' =>"TIM###Brazil",
        '724F03' =>"TIM###Brazil",
        '724F05' =>"CL###Brazil",
        '724F31' =>"Oi###Brazil",
        '528F01' =>"Jabatan Telekom###Brunei Darussalam",
        '528F11' =>"DST Communications###Brunei Darussalam",
        '284F01' =>"MobilTel AD###Bulgaria",
        '284F05' =>"Globul###Bulgaria",
        '613FFF' =>"OnaTel###Burkina Faso",
        '456F01' =>"CamGSM###Cambodia",
        '456F02' =>"Cambodia Samart Comms###Cambodia",
        '456FFF' =>"Cambodia Shinawatra###Cambodia",
        '624F01' =>"PTT Cameroon Cellnet###Cameroon",
        '302F37' =>"Microcell Connexions Inc###Canada",
        '302720' =>"Rogers###Canada",
        '625F01' =>"Cabo Verde Telecom###Cape Verde",
        '730F01' =>"Entel Telefonia Movil###Chile",
        '730F02' =>"Movistar Chile###Chile",
        '730F03' =>"Claro Chile###Chile",
        '730F10' =>"Entel PCS Telecom.###Chile",
        '460F00' =>"Guangdong MCC / CMCC / NVyR / China Telecom GSM###China",
        '460F01' =>"China Unicom GSM###China",
        '460F02' =>"Liaoning PPTA###China",
        '460FFF' =>"Beijing Wireless###China",
        '460FFF' =>"Zhuhai Comms###China",
        '460FFF' =>"DGT MPT###China",
        '460FFF' =>"Jiaxing PTT###China",
        '460FFF' =>"Tjianjin Toll###China",
        '629FFF' =>"African Telecoms###Congo",
        '629FFF' =>"Congolaise Wireless###Congo",
        '219F01' =>"Croatian Telecoms Cronet / HTmobile H###Croatia",
        '219F10' =>"Vipnet###Croatia",
        '280F01' =>"Cyprus Telecoms Authority###Cyprus",
        '280F10' =>"areeba###Cyprus",
        '230F01' =>"Radio Mobil / T Mobile / Pegas###Czech Republic",
        '230F02' =>"Eurotel Praha###Czech Republic",
        '230F03' =>"Oskar / SPT Telecom###Czech Republic",
        '238F01' =>"TDC / Tele-Danmark Mobil###Denmark",
        '238F02' =>"Sonofon###Denmark",
        '238F06' =>"3 / Hutchsion 3G (UMTS)###Denmark",
        '238F20' =>"Telia###Denmark",
        '238F30' =>"Orange / Mobilix###Denmark",
        '602F01' =>"MobiNil###Egypt",
        '602F02' =>"Misrfone Telecom. Click GSM / Vodafone###Egypt",
        '248F01' =>"Estonian Mobile Telecom###Estonia",
        '248F02' =>"Radiolinja Eesti###Estonia",
        '248F03' =>"Tele 2 / Q GSM###Estonia",
        '636F01' =>"Etheopian Telecoms Auth.###Ethiopia",
        '288FFF' =>"Faroese Telecom###Faroe Islands",
        '542F01' =>"Vodafone###Fiji",
        '244F03' =>"Telia###Finland",
        '244F05' =>"Elisa / Radiolinja / Alands Mobiltelefon###Finland",
        '244F09' =>"Finnet Group / Lnnen Puhelin / Helsingin Puhelin###Finland",
        '244F12' =>"FinNet###Finland",
        '244F91' =>"Sonera Corporation###Finland",
        '208F01' =>"Orange / France Telecom Itineris###France",
        '208F10' =>"SFR###France",
        '208F20' =>"Bouygues Telecom###France",
        '547F20' =>"Tikiphone###French Polynesia (France)",
        '340F01' =>"France Caraibe Ameris###French West Indies (France)",
        '282F01' =>"Geocell Limited###Georgia",
        '282F02' =>"Magticom GSM###Georgia",
        '282FFF' =>"Superphone###Georgia",
        '262F01' =>"D1 - DeTe Mobil / T-Mobile###Germany",
        '262F02' =>"D2 - Mannesmann Mobilfunk / Vodafone D2###Germany",
        '262F03' =>"E-Plus Mobilfunk###Germany",
        '262F07' =>"Viag Interkom / O2###Germany",
        '620F01' =>"ScanCom###Ghana",
        '620FFF' =>"Franci Walker Ltd###Ghana",
        '266F01' =>"Gibraltar Telecoms GibTel###Gibraltar",
        '202F01' =>"Cosmote###Greece",
        '202F05' =>"Panafon / Vodafone###Greece",
        '202F09' =>"Q-telecom###Greece",
        '202F10' =>"STET / TIM###Greece",
        '290F01' =>"Tele Greenland###Greenland",
        '340F01' =>"F Orange###Guadeloupe (France)",
        '611F02' =>"Sotelgui Lagui###Guinea",
        '611FFF' =>"Int'l Wireless###Guinea",
        '611FFF' =>"Spacetel###Guinea",
        '454F00' =>"Hong Kong Telecom CSL###Hong Kong",
        '454F04' =>"Hutchison Telecom / Orange###Hong Kong",
        '454F06' =>"SmarTone Mobile Comms###Hong Kong",
        '454F10' =>"New World PCS###Hong Kong",
        '454F12' =>"Peoples Telephone###Hong Kong",
        '454F16' =>"Mandarin Comm / Sunday###Hong Kong",
        '454F18' =>"Pacific Link###Hong Kong",
        '454F22' =>"P Plus Comm / SMC PCS###Hong Kong",
        '216F01' =>"Pannon GSM###Hungary",
        '216F30' =>"Westel / T-Mobile (GSM 900)###Hungary",
        '216F70' =>"Vodafone HU###Hungary",
        '274F01' =>"Iceland Telecom Siminn###Iceland",
        '274F02' =>"TAL hf / Og Vodafon###Iceland",
        '404F02' =>"AirTel###India",
        '404F03' =>"INA Airtel###India",
        '404F04' =>"IDEA###India",
        '404F05' =>"Hutch###India",
        '404F07' =>"TATA Cellular###India",
        '404F10' =>"Bharti Cellular Telecom Airtel###India",
        '404F11' =>"Essar / Sterling Cellular###India",
        '404F12' =>"Escotel Mobile Comms###India",
        '404F13' =>"Hutch###India",
        '404F14' =>"Modi Telstra Modicom###India",
        '404F15' =>"Aircel Digilink Essar Cellph.###India",
        '404F20' =>"Hutchison Maxtouch / Orange###India",
        '404F21' =>"BPL Mobile###India",
        '404F22' =>"Idea###India",
        '404F27' =>"BPL USWest Cellular / Cellular Comms###India",
        '404F30' =>"Usha Martin Tel. Command###India",
        '404F31' =>"AirTel / Mobilenet###India",
        '404F40' =>"AirTel / SkyCell Communications###India",
        '404F41' =>"RPG MAA###India",
        '404F42' =>"Srinivas Cellcom###India",
        '404F45' =>"AirTel###India",
        '404F49' =>"IND AIRTEL###India",
        '404F53' =>"BSNL MOBIL###India",
        '404F55' =>"BSNL MOBIL###India",
        '404F62' =>"BSNL MOBIL###India",
        '404F69' =>"DOLPHIN###India",
        '404F70' =>"AirTel###India",
        '404F75' =>"CellOne###India",
        '404F86' =>"Hutch###India",
        '404FFF' =>"Mobile Telecom###India",
        '404FFF' =>"Airtouch###India",
        '404FFF' =>"BPL USWest###India",
        '404FFF' =>"Koshika###India",
        '404FFF' =>"Bharti Telenet###India",
        '404FFF' =>"Birla Comm###India",
        '404FFF' =>"JT Mobiles###India",
        '404FFF' =>"Evergrowth Telecom###India",
        '404FFF' =>"Hexacom India###India",
        '404FFF' =>"Reliance Telecom###India",
        '404FFF' =>"Fascel Limited###India",
        '404F90' =>"AirTel###India",
        '404F92' =>"AirTel###India",
        '404F93' =>"AirTel###India",
        '404F95' =>"AirTel###India",
        '404F96' =>"AirTel###India",
        '404F97' =>"AirTel###India",
        '404F98' =>"AirTel###India",
        '510F01' =>"PT Satelindo / PT Satelit Palapa / INDOSAT###Indonesia",
        '510F10' =>"Telkomsel###Indonesia",
        '510F11' =>"PT Excelcomindo Excelcom / proXL###Indonesia",
        '510F21' =>"IND IM3 / INDOSAT###Indonesia",
        '432F11' =>"T.C.I.###Iran",
        '432FFF' =>"Celcom###Iran",
        '432FFF' =>"Kish Free Zone###Iran",
        '418F01' =>"Iraq Telecom###Iraq",
        '272F01' =>"Eircell / Vodafone###Ireland",
        '272F02' =>"Esat Digifone / O2###Ireland",
        '272F03' =>"Meteor###Ireland",
        '425F01' =>"Partner Communications / Orange###Israel",
        '425F02' =>"Cellcom###Israel",
        '222F01' =>"Telecom Italia Mobile###Italy",
        '222F10' =>"Omnitel Pronto / Vodafone###Italy",
        '222F88' =>"Wind Telecomunicazioni###Italy",
        '612F01' =>"Comstar Cellular Network###Ivory Coast",
        '612F02' =>"Telecel###Ivory Coast",
        '612F03' =>"S.I.M. Ivoiris###Ivory Coast",
        '612F05' =>"Loteny Telecom Telecel###Ivory Coast",
        '338F05' =>"Digicel###Jamaica",
        '338180' =>"C&W###Jamaica",
        '440FFF' =>"Japan",
        '160FF1' =>"J.M.T.S. Fastlink###Jordan",
        '167FF7' =>"MobCom###Jordan",
        '401F01' =>"K-Mobile###Kazakhstan",
        '401F02' =>"K'Cell###Kazakhstan",
        '639FFF' =>"Kenya Telecom###Kenya",
        '450F05' =>"SK Telecom###Korea",
        '419F02' =>"Mobile Telecoms / MTCNet###Kuwait",
        '419F03' =>"Wataniya###Kuwait",
        '437F01' =>"Bitel Ltd###Kyrgyz Republic",
        '457F01' =>"Lao Shinawatra Telecom###Laos",
        '247F01' =>"Latvian Mobile Tel.###Latvia",
        '247F02' =>"BALTCOM GSM / Tele 2###Latvia",
        '415F01' =>"FTML Cellis###Lebanon",
        '415F03' =>"LibanCell###Lebanon",
        '651F01' =>"Vodacom###Lesotho",
        '618F01' =>"Omega Communications###Liberia",
        '228F01' =>"Natel-D###Liechtenstein",
        '246F01' =>"Omnitel###Lithuania",
        '246F02' =>"UAB Bite GSM###Lithuania",
        '246F03' =>"Tele 2###Lithuania",
        '270F01' =>"P&T LUXGSM###Luxembourg",
        '270F77' =>"Millicom Lux' S.A / Millicom Tango GSM###Luxembourg",
        '455F01' =>"C.T.M. Telemovel+###Macao",
        '294F01' =>"Mecedonian Tel. / PTT Makedonija / Mobimak###Macedonia",
        '646F01' =>"Madacom###Madagascar",
        '646F02' =>"SMM Antaris###Madagascar",
        '646F03' =>"Sacel###Madagascar",
        '650F01' =>"TNL / Telecom Network Callpoint###Malawi",
        '502F02' =>"My BSB###Malaysia",
        '502F03' =>"Binariang###Malaysia",
        '502F12' =>"Binariang Comms. Maxis###Malaysia",
        '502F13' =>"Telekom Cellular TM Touch###Malaysia",
        '502F16' =>"DiGi Telecommunications###Malaysia",
        '502F17' =>"Time Wireless Adam###Malaysia",
        '502F19' =>"Celcom###Malaysia",
        '278F01' =>"Telecell###Malta",
        '278FFF' =>"Advanced###Malta",
        '604F01' =>"O.N.P.T###Marocco",
        '617F01' =>"Cellplus###Mauritius",
        '334F20' =>"Telcel###Mexico",
        '259F01' =>"Voxtel###Moldova",
        '259F02' =>"Moldcell###Moldova",
        '208F01' =>"France Telecom###Monaco",
        '208F10' =>"SFR###Monaco",
        '208FFF' =>"Office des Telephones###Monaco",
        '428F99' =>"Mobicom###Mongolia",
        '220F02' =>"ProMonte###Montenegro",
        '220F04' =>"T-Mobile Montenegro###Montenegro",
        '604F01' =>"Itissalat Al-Maghrib / IAM###Morocco",
        '634F01' =>"Telecom de Mocambique (GSM 1800)###Mozambique",
        '649F01' =>"MTC###Namibia",
        '204F04' =>"Libertel / Vodafone###Netherlands",
        '204F08' =>"KPN Telecom###Netherlands",
        '204F12' =>"Telfort Holding NV###Netherlands",
        '204F16' =>"Ben###Netherlands",
        '204F20' =>"Dutchtone / Orange###Netherlands",
        '362F91' =>"CHIPP###Netherland Antilles (Netherlands)",
        '546F01' =>"OPT Mobilis###New Caledonia",
        '530F01' =>"Bell South / Vodafone###New Zealand",
        '530F03' =>"Telecom NZ###New Zealand",
        '530F04' =>"Telstra###New Zealand",
        '621F30' =>"MTN###Nigeria",
        '621FFF' =>"EMIS###Nigeria",
        '242F01' =>"TeleNor Mobil###Norway",
        '242F02' =>"NetCom GSM###Norway",
        '422F02' =>"General Telecoms###Oman",
        '410F01' =>"Mobilink###Pakistan",
        '410F03' =>"UFONE###Pakistan",
        '410F04' =>"Paktel###Pakistan",
        '410F06' =>"Telenor###Pakistan",
        '410F07' =>"Warid###Pakistan",
        '310F01' =>"Pacific Mobile Comms###Papua New Guinea",
        '310F01' =>"VOX###Paraguay",
        '515F01' =>"Islacom / Isla Comms###Philippines",
        '515F02' =>"Globe Telecom###Philippines",
        '515F03' =>"Smart Communications###Philippines",
        '260F01' =>"Polkomtel Plus GSM###Poland",
        '260F02' =>"ERA GSM###Poland",
        '260F03' =>"IDEA Centertel###Poland",
        '268F01' =>"Telecel Communicacoes / Vodafone###Portugal",
        '268F03' =>"Optimus Telecom.###Portugal",
        '268F06' =>"Telecom Moveis Nac. / TMN###Portugal",
        '268FFF' =>"Main Road Telecoms###Portugal",
        '427F01' =>"Q-Net / Q-Tel QATARNET###Qatar",
        '647F10' =>"Societe Reunionnaise SRR###Reunion",
        '226F01' =>"MobiFon Connex GSM###Romania",
        '226F10' =>"Mobil Rom Dialog / Orange###Romania",
        '250F01' =>"Mobile Tele Moscow / MTS-Tatars / MTS Moscow###Russian Federation",
        '250F02' =>"North West GSM / NW GSM / St. Petersburg / MegaFon###Russian Federation",
        '250F03' =>"NCC###Russian Federation",
        '250F05' =>"Siberian Cellular / SCS / Yenis / Yeniseytel###Russian Federation",
        '250F07' =>"BM Telecom / Zao SMARTS###Russian Federation",
        '250F10' =>"Dontelekom / Don Telecom###Russian Federation",
        '250F12' =>"New Telephone Company / Far Eastern Cellular###Russian Federation",
        '250F13' =>"Kuban GSM###Russian Federation",
        '250F14' =>"Di-ex###Russian Federation",
        '250F16' =>"NTC###Russian Federation",
        '250F17' =>"Ermak RMS###Russian Federation",
        '250F19' =>"RUS: INDIG###Russian Federation",
        '250F20' =>"TELE2###Russian Federation",
        '250F28' =>"Extel###Russian Federation",
        '250F39' =>"Uraltel###Russian Federation",
        '250F44' =>"North Caucasian GSM###Russian Federation",
        '250F99' =>"KB Impuls BeeLine###Russian Federation",
        '635F10' =>"Rwandacell###Rwanda",
        '222F10' =>"Omnitel###San Marino",
        '222F01' =>"Telecom Italia Mobile###San Marino",
        '222F88' =>"Wind###San Marino",
        '420F01' =>"Ministry of PTT Al Jawal###Saudi Arabia",
        '420F07' =>"Electronics App' Est. / EAE###Saudi Arabia",
        '608F01' =>"Sonatel ALIZE###Senegal",
        '633F01' =>"Seychelles Cellular Services###Seychelles",
        '220F01' =>"Mobtel###Serbia",
        '220F03' =>"YU-03###Serbia",
        '633F10' =>"Telecom Airtel###Seychelles",
        '525F01' =>"Singapore Telecom / SingTel (GSM 900)###Singapore",
        '525F02' =>"Singapore Telecom / SingTel (GSM 1800)###Singapore",
        '525F03' =>"MobileOne Asia / SGP-M1-3GS###Singapore",
        '525F05' =>"StarHub###Singapore",
        '525FFF' =>"Binariang###Singapore",
        '231F01' =>"Orange / Globtel GSM###Slovakia",
        '231F02' =>"Eurotel GSM###Slovakia",
        '293F40' =>"Si.Mobil###Slovenia",
        '293F41' =>"Mobitel###Slovenia",
        '655F01' =>"Vodacom###South Africa",
        '655F07' =>"Cell C###South Africa",
        '655F10' =>"MTN###South Africa",
        '214F01' =>"Vodafone###Spain",
        '214F03' =>"Orange###Spain",
        '214F04' =>"Xfera (planned)###Spain",
        '214F05' =>"Movistar (for resellers, planned)###Spain",
        '214F06' =>"Vodafone (for resellers, planned)###Spain",
        '214F07' =>"Movistar###Spain",
        '214F08' =>"Euskaltel (MVNO, planned)###Spain",
        '214F09' =>"Orange (for resellers, planned)###Spain",
        '413F02' =>"MTN Networks Pvt Ltd / Dialog###Sri Lanka",
        '634F01' =>"Mobile Telephone Company / Mobitel###Sudan",
        '653FFF' =>"Swaziland",
        '240F01' =>"Telia Mobile###Sweden",
        '240F02' =>"3 / Hutchison 3G (UMTS)###Sweden",
        '240F07' =>"Comviq GSM###Sweden",
        '240F08' =>"Telenor (was Vodafone which was Europolitan)###Sweden",
        '228F01' =>"Swisscom NATEL (GSM 900/1800)###Switzerland",
        '228F02' =>"diAx mobile / Sunrise###Switzerland",
        '228F03' =>"Orange###Switzerland",
        '417F01' =>"SyriaTel###Syria",
        '417F09' =>"Syrian Telecom Est. MOBILE###Syria",
        '466F01' =>"FarEasTone / Far EasTone Telecoms###Taiwan",
        '466F06' =>"Tuntex Telecom###Taiwan",
        '466F88' =>"KG Telecom###Taiwan",
        '466F89' =>"VIBO Telecom###Taiwan",
        '466F92' =>"LDTA###Taiwan",
        '466F93' =>"Mobitai Communications###Taiwan",
        '466F97' =>"Pacific Cellular TWN GSM###Taiwan",
        '466F99' =>"TransAsia Telecoms###Taiwan",
        '466F11' =>"Chunghwa###Taiwan",
        '640F01' =>"Tritel###Tanzania",
        '640F05' =>"Celtel###Tanzania",
        '520F01' =>"Advanced Info Service AIS GSM###Thailand",
        '520F10' =>"WCS IQ###Thailand",
        '520F18' =>"Total Access Comms / Total Access Worldphone###Thailand",
        '520F23' =>"Digital Phone Hello###Thailand",
        '520F99' =>"True (formally Orange)###Thailand",
        '615F01' =>"Togo Telecom TOGO CELL###Togolese Republic",
        '347F12' =>"TSTT###Trinidad and Tobago",
        '605F02' =>"Tunisian PTT / Tunisie Telecom Tunicell###Tunisia",
        '286F01' =>"Turk Telekom Turkcell###Turkey",
        '286F02' =>"Telsim Mobil Telekom.###Turkey",
        '286F03' =>"Aria###Turkey",
        '641F01' =>"Celtel Cellular###Uganda",
        '641F10' =>"MTN Uganda###Uganda",
        '255F01' =>"Ukrainian Mobile Comms / WellCOM###Ukraine",
        '255F02' =>"Ukrainian Radio Systems###Ukraine",
        '255F03' =>"Kyivstar JSC GSM###Ukraine",
        '255F05' =>"Golden Telecom###Ukraine",
        '255F06' =>"UA ASTELIT / life:)###Ukraine",
        '424F01' =>"UAE ETISALAT-G1###United Arab Emirates",
        '424F02' =>"UAE ETISALAT-G2###United Arab Emirates",
        '234F00' =>"Virgin 3510i###United Kingdom",
        '234F10' =>"Cellnet / O2###United Kingdom",
        '234F15' =>"Vodafone###United Kingdom",
        '234F20' =>"Hutchison 3G (3)###United Kingdom",
        '234F30' =>"One 2 One / T-Mobile###United Kingdom",
        '234F31' =>"Virgin###United Kingdom",
        '234F32' =>"Virgin###United Kingdom",
        '234F33' =>"Orange###United Kingdom",
        '234F50' =>"Jersey Telecom GSM###United Kingdom",
        '234F55' =>"Guernsey Telecom GSM###United Kingdom",
        '234F58' =>"Manx Telecom GSM###United Kingdom",
        '310000' =>"Mid-Tex Celular###United States of America",
        '310012' =>"Verizon Wireless###United States of America",
        '310013' =>"MobileTel###United States of America",
        '310020' =>"APC Sprint Spectrum###United States of America",
        '310090' =>"Edge Wireless###United States of America",
        '310110' =>"Wireless 2000 Telephone###United States of America",
        '310150' =>"Bell South / BellSouth Mobility DCS / Cingular###United States of America",
        '310160' =>"T-Mobile / Omnipoint Communications###United States of America",
        '310170' =>"Pacific Bell Wireless / Cingular / T-Mobile###United States of America",
        '310180' =>"Cingular###United States of America",
        '310190' =>"Alaska Telecom Mobile###United States of America",
        '310200' =>"T-Mobile Idaho / Oregon / Washington State###United States of America",
        '310210' =>"T-Mobile Iowa###United States of America",
        '310220' =>"T-Mobile Kansas / Oklahoma###United States of America",
        '310230' =>"T-Mobile Utah###United States of America",
        '310240' =>"T-Mobile New Mexico / Texas / Arizona###United States of America",
        '310250' =>"T-Mobile Hawaii###United States of America",
        '310260' =>"Western Wireless / Voicestream / T-Mobile###United States of America",
        '310270' =>"Powertel / T-Mobile###United States of America",
        '310280' =>"T-Mobile###United States of America",
        '310290' =>"T-Mobile###United States of America",
        '310300' =>"T-Mobile###United States of America",
        '310310' =>"Aerial Communications / T-Mobile Florida###United States of America",
        '310380' =>"AT&T Wireless / Cingular###United States of America",
        '310410' =>"Cingular###United States of America",
        '310770' =>"Iowa Wireless Services###United States of America",
        '434F01' =>"Buztel###Uzbekistan",
        '434F04' =>"Daewoo Unitel GSM###Uzbekistan",
        '434F05' =>"Coscom###Uzbekistan",
        '434F07' =>"UXB-UZD###Uzbekistan",
        '222F10' =>"Omnitel###Vatican",
        '222F01' =>"Telecom Italia Mobile###Vatican",
        '222F88' =>"Wind###Vatican",
        '734F01' =>"Infonet###Venezuela",
        '734F02' =>"Digitel###Venezuela",
        '452F01' =>"MTSC / Mobifon###Vietnam",
        '452F02' =>"DGPT###Vietnam",
        '421F01' =>"SabaFon###Yemen",
        '630FFF' =>"African Telecom Net###Zaire",
        '645F01' =>"Zamcell###Zambia",
        '648F01' =>"NET*ONE###Zimbabwe",
        '648F04' =>"Telecel###Zimbabwe",
        'FFFFFF' =>"Empty"
          );

#hash header dedicated file
my %hd_df = (
    '1-2' => "(1-2) \tRFU: ",
    '3-4' => "(3-4) \tMemory allocated at Master File (byte): ",
    '5-6' => "(5-6) \tFile ID: ",
    '7'   => "(7) \tType of file: ",
    '8-12'=> "(8-12) \tRFU: ",
    '13'  => "(13) \tLength of the following data: ",
    '14'  => "(14) \tFile characteristics: ",
    '15'  => "(15) \tNumber of DFs child of current directory: ",
    '16'  => "(16) \tNumber of EFs child of current directory: ",
    '17'  => "(17) \tNumber of CHVs, UNBLOCK CHVs and administrative codes: ",
    '18'  => "(18) \tRFU: ",
    '19'  => "(19) \tCHV1 status: ",
    '20'  => "(20) \tUNBLOCKED CHV1 status: ",
    '21'  => "(21) \tCHV2 status: ",
    '22'  => "(22) \tUNBLOCKED CHV2 status: ",
    '23'  => "(23) \tRFU: "
     );


#hash header elementary file
my %hd_ef = (
    '1-2' => "(1-2) \tRFU: ",
    '3-4' => "(3-4) \tFile size (byte): ",
    '5-6' => "(5-6) \tFile ID: ",
    '7'   => "(7) \tType of file: ",
    '8'   => "(8) \tRFU: ",
    '9-11'=> "(9-11) \tAccess condition: ",
    '12'  => "(12) \tFile status ",
    '13'  => "(13) \tLength of the following data: ",
    '14'  => "(14) \tStructure of EF: ",
    '15'  => "(15) \tLength of a record: ",
    '16'  => "(16) \tRFU: "  
    );
    
my %months =   (
                '01' => "Jan",
                '02' => "Feb",
                '03' => "Mar",
                '04' => "Apr",
                '05' => "May",
                '06' => "Jun",
                '07' => "Jul",
                '08' => "Aug",
                '09' => "Sep",
                '10' => "Oct",
                '11' => "Nov",
                '12' => "Dec"
                );

# Hash file Data structure for Header file
my %file_type_df = ('00' => "RFU", '01' => "MF", '02' => "DF", '04' => "EF");
my %file_struct = ('00' => "transparent", '01' => "linear fixed", '03' => "cyclic");
# Hash table for Access Condition
my %ac = (  '0' => 'ALW', '1' => 'CHV1','2' => 'CHV2',
            '3' => 'RFU', '4' => 'ADM', '5' => 'ADM',
            '6' => 'ADM', '7' => 'ADM', '8' => 'ADM',
            '9' => 'ADM', 'A' => 'ADM', 'B' => 'ADM',
            'C' => 'ADM', 'D' => 'ADM', 'E' => 'ADM',
            'F' => 'NEV');

# Hash EFs features
 my %file_c = (  
    'b134' => {
        '100' => "clock stop allowed, no preferred level",
        '110' => "clock stop allowed, high level preferred",
        '101' => "clock stop allowed, low level preferred",
        '000' => "clock stop not allowed",
        '010' => "clock stop not allowed, unless at high level",
        '001' => "clock stop not allowed, unless at low level"
    },
            
    'b2'   => {
        '0' => "Frequency required 13/8 MHz",
        '1' => "Frequency required 13/4 MHz"        
    },       
            
    'b5'   => {
            '0' => "5V only SIM",
            '1' => "3V technology SIM"
    },
                     
    'b67'  => "RFU",
       
    'b8'   => {
        '0' => "CHV1 enabled",
        '1' => "CHV1 disabled"    
    }                
);

# Hash standard EFs with full path 
 my %standard_file =(                     
  '3F00' => "MF",
  '3F00/2FE2' => "EFICCID",
  '3F00/2F05' => "EFELP",

# 'DF => 7F20'
  '3F00/7F20' => "DFGSM",
  '3F00/7F20/5F30' => "DFIRIDIUM",
  '3F00/7F20/5F31' => "DFGLOBST",
  '3F00/7F20/5F32' => "DFICO",
  '3F00/7F20/5F33' => "DFACeS",
  '3F00/7F20/5F40' => "DFEIA/TIA-553",
  '3F00/7F20/5F60' => "DFCTS",
  '3F00/7F20/5F70' => "DFSoLSA",
  
  '3F00/7F20/5F70/4F30' => "EFSAI",
  '3F00/7F20/5F70/4F31' => "EFSLL",
  
  '3F00/7F20/5F3C' => "DFMExE",
  
  '3F00/7F20/5F3C/4F40' => "EFMExE-ST",
  '3F00/7F20/5F3C/4F41' => "EFORPK",
  '3F00/7F20/5F3C/4F42' => "EFARPK",
  '3F00/7F20/5F3C/4F43' => "EFTPRPK",
  '3F00/7F20/6F05' => "EFLP",
  '3F00/7F20/6F07' => "EFIMSI",
  '3F00/7F20/6F20' => "EFKc",
  '3F00/7F20/6F30' => "EFPLMNsel",
  '3F00/7F20/6F31' => "EFHPLMN",
  '3F00/7F20/6F37' => "EFACMm",
  '3F00/7F20/6F38' => "EFSST",
  '3F00/7F20/6F39' => "EFACM",
  '3F00/7F20/6F3E' => "EFGID1",
  '3F00/7F20/6F3F' => "EFGID2",
  '3F00/7F20/6F41' => "EFPUCT",
  '3F00/7F20/6F45' => "EFCBMI",  
  '3F00/7F20/6F46' => "EFSPN",
  '3F00/7F20/6F48' => "EFCBMID",
  '3F00/7F20/6F74' => "EFBCCH",	
  '3F00/7F20/6F78' => "EFACC",
  '3F00/7F20/6F7B' => "EFFPLMN",
  '3F00/7F20/6F7E' => "EFLOCI",
  '3F00/7F20/6FAD' => "EFAD",
  '3F00/7F20/6FAE' => "EFPHASE",
  '3F00/7F20/6FB1' => "EFVGCS",
  '3F00/7F20/6FB2' => "EFVGCSS",
  '3F00/7F20/6FB3' => "EFVBS",
  '3F00/7F20/6FB4' => "EFVB",
  '3F00/7F20/6FB5' => "EFeMLPP",
  '3F00/7F20/6FB6' => "EFAAeM",
  '3F00/7F20/6FB7' => "EFECC",
  '3F00/7F20/6F50' => "EFCBMIR",
  '3F00/7F20/6F51' => "EFNIA",
  '3F00/7F20/6F52' => "PEFKcG",
  '3F00/7F20/6F53' => "EFLOCIGPRS",
  '3F00/7F20/6F54' => "EFSUME",
  '3F00/7F20/6F60' => "EFPLMNwACT",
  '3F00/7F20/6F61' => "EFOPLMNwACT",
  '3F00/7F20/6F62' => "EFHPLMNwACT",
  '3F00/7F20/6F63' => "EFCPBC",
  '3F00/7F20/6F64' => "EFINVSCAN",
  
# 'DF => 7F10'
  '3F00/7F10' => "DFTELECOM",
  '3F00/7F10/5F50' => "DFGRAPHICS",
 
  '3F00/7F10/5F50/4F20' => "EFIMG",
  '3F00/7F10/6F3A' => "EFADN",
  '3F00/7F10/6F3B' => "EFFDN",
  '3F00/7F10/6F3C' => "EFSMS",
  '3F00/7F10/6F3D' => "EFCCP",
  '3F00/7F10/6F40' => "EFMSISDN",
  '3F00/7F10/6F42' => "EFSMSP",
  '3F00/7F10/6F43' => "EFSMSS",
  '3F00/7F10/6F44' => "EFLND",
  '3F00/7F10/6F47' => "EFSMSR",
  '3F00/7F10/6F49' => "EFS",
  '3F00/7F10/6F4A' => "EFEXT1",
  '3F00/7F10/6F4B' => "EFEXT2",
  '3F00/7F10/6F4C' => "EFEXT3",
  '3F00/7F10/6F4D' => "EFBDN",
  '3F00/7F10/6F4E' => "EFEX",
  
  '3F00/7F22' => "DFIS-41",
  '3F00/7F23' => "DFFP-CTS"
);

# Hash standard EFs without path
my %standard_file2 =(
  '3F00' => 'MF',
# EF under MF (3F00)
  '2FE2' => 'ICCID',
  '2F05' => 'ELP',

# 'DF => 7F20' (under MF)
  '7F20' => 'DFGSM',
  '5F30' => 'DFIRIDIUM',
  '5F31' => 'DFGLOBST',
  '5F32' => 'DFICO',
  '5F33' => 'DFACeS',
  '5F40' => 'DFEIA/TIA-553',
  '5F60' => 'DFCTS',
  '5F70' => 'DFSoLSA',
  
# 'DF => 5F70' under 7F20  
  '4F30' => 'SAI',
  '4F31' => 'SLL',
  
  '5F3C' => 'DFMExE',
  
  '4F40' => 'MExEST',
  '4F41' => 'ORPK',
  '4F42' => 'ARPK',
  '4F43' => 'TPRPK',
  
# 'EF under 7F20'  
  '6F05' => 'LP',
  '6F07' => 'IMSI',
  '6F20' => 'KC',
  '6F30' => 'PLMNsel',
  '6F31' => 'HPLMN',
  '6F37' => 'ACMmax',
  '6F38' => 'SST',
  '6F39' => 'ACM',
  '6F3E' => 'GID1',
  '6F3F' => 'GID2',
  '6F41' => 'PUCT',
  '6F45' => 'CBMI',  
  '6F46' => 'SPN',
  '6F48' => 'CBMID',
  '6F74' => 'BCCH',	
  '6F78' => 'ACC',
  '6F7B' => 'FPLMN',
  '6F7E' => 'LOCI',
  '6FAD' => 'AD',
  '6FAE' => 'PHASE',
  '6FB1' => 'VGCS',
  '6FB2' => 'VGCSS',
  '6FB3' => 'VBS',
  '6FB4' => 'VBSS',
  '6FB5' => 'eMLPP',
  '6FB6' => 'AAeM',
  '6FB7' => 'ECC',
  '6F50' => 'CBMIR',
  '6F51' => 'NIA',
  '6F52' => 'KCGPRS',
  '6F53' => 'LOCIGPRS',
  '6F54' => 'SUME',
  '6F60' => 'PLMNwACT',
  '6F61' => 'OPLMNwACT',
  '6F62' => 'HPLMNwACT',
  '6F63' => 'CPBCCH',
  '6F64' => 'INVSCAN',
  
# 'DF => 7F10'
  '7F10' => 'DFTELECOM',
  '5F50' => 'DFGRAPHICS',
 
  '4F20' => 'IMG',
  '6F3A' => 'ADN',
  '6F3B' => 'FDN',
  '6F3C' => 'SMS',
  '6F3D' => 'CCP',
  '6F40' => 'MSISDN',
  '6F42' => 'SMSP',
  '6F43' => 'SMSS',
  '6F44' => 'LND',
  '6F47' => 'SMSR',
  '6F49' => 'SDN',
  '6F4A' => 'EXT1',
  '6F4B' => 'EXT2',
  '6F4C' => 'EXT3',
  '6F4D' => 'BDN',
  '6F4E' => 'EXT4',
  
  '7F22' => 'DFIS-41',
  '7F23' => 'DFFP-CTS'
);

# Hash File for Elementary File Description 
my %ef_description = (

# EFs of MF '3F 00'
ICCID         => "EFICCID '2FE2' (ICC Identification): This EF provides a unique identification number for the SIM.",

ELP           => "EFELP '2F05' (Extended language preference): This EF contains the codes for up to n languages.
                  This information, determined by the user/operator, defines the preferred languages of the user
                  in order of priority. This information may be used by the ME for MMI purposes.",
                  
# EFs of DF '7F 20'
LP            => "EF LP '6F05' (Language preference): This EF contains the codes for one or more languages.
                  This information, determined by the user/operator, defines the preferred languages of the user
                  in order of priority. This information may be used by the ME for MMI purposes.",
                  
IMSI          => "EF IMSI '6F07' (IMSI): This EF contains the International Mobile Subscriber Identity.",

KC            => "EF Kc '6F20' (Chipering Key): This EF contains the ciphering key Kc and the ciphering key sequence number n.",

PLMNsel      => "EF PLMNsel '6F30' (PLMN selector): This information determined by the user/operator defines the preferred
                  PLMNs of the user in priority order.",
                  
HPLMN         => "EF HPLMN '6F31' (Higher Priority PLMN search period): This EF contains the interval of time between searches
                  for the PLMN.",
                  
ACMmax        => "EF ACMmax '6F37' (ACM maximum value): This EF contains the maximum value of the Accumulated Call Meter (ACM).",

SST           => "EF SST '6F38' (SIM service table): This EF indicates which services are allocated, and whether,
                  if allocated, the service is activated. If a service is not allocated or not activated in the SIM,
                  the ME shall not select this service.",
                  
ACM           => "EF ACM '6F39' (Accumulated call meter): This EF contains the total number of units for both the current
                  call and the preceding calls.",
                  
GID1          => "EF GID1 '6F3E' (Group Identifier Level 1): This EF contains identifiers for particular SIM-ME associations.
                  It can be used to identify a group of SIMs for a particular application.",
                 
GID2          => "EF GID2 '6F3F' (Group Identifier Level 2): This EF contains identifiers for particular SIM-ME associations.
                  It can be used to identify a group of SIMs for a particular application.",
                  
PUCT          => "EF PUCT '6F41' (Price per unit and currency table): This EF contains the Price per Unit and Currency Table
                  (PUCT). The PUCT is Advice of Charge related information which may be used by the ME in conjunction
                  with EFACM to compute the cost of calls in the currency chosen by the subscriber, as specified in
                  TS 22.024 [7]. This EF shall always be allocated if EFACM is allocated.",
                  
CBMI          => "EF CBMI '6F45' (Cell broadcast message identifier selection): This EF contains the Message Identifier
                  Parameters which specify the type of content of the cell broadcast messages that the subscriber
                  wishes the MS to accept. Any number of CB Message Identifier Parameters may be stored in the SIM.
                  No order of priority is applicable.",

SPN           => "EF SPN '6F46' (Service Provider Name): This EF contains the service provider name and appropriate requirements
                  for the display by the ME.",
                  
CBMID         => "EF CBMID '6F48' (Cell Broadcast Message Identifier for Data Download): This EF contains the message
                  identifier parameters which specify the type of content of the cell broadcast messages
                  which are to be passed to the SIM.",
                  
BCCH          => "EF BCCH '6F74' (Broadcast control channels): This EF contains information concerning the BCCH according to
                  TS 04.08 [15]. BCCH storage may reduce the extent of a Mobile Station's search of BCCH carriers when
                  selecting a cell. The BCCH carrier lists in an MS shall be in accordance with the procedures specified
                  in TS 04.08 [15]. The MS shall only store BCCH information from the System Information 2 message and
                  not the 2bis extension message.",
                  
ACC           => "EF ACC '6F78' (Access control class): This EF contains the assigned access control class(es). TS 22.011 [5]
                  refers. The access control class is a parameter to control the RACH utilization. 15 classes are split
                  into 10 classes randomly allocated to normal subscribers and 5 classes allocated to specific high
                  priority users. For more information see TS 22.011 [5].",
                  
FPLMN         => "EF FPLMN '6F7B' (Forbidden PLMNs): This EF contains the coding for four Forbidden PLMNs (FPLMN). It is read
                  by the ME as part of the SIM initialization procedure and indicates PLMNs which the MS shall not
                  automatically attempt to access. A PLMN is written to the EF if a network rejects a Location Update
                  with the cause 'PLMN not allowed'. The ME shall manage the list as follows. When four FPLMNs are held
                  in the EF, and rejection of a further PLMN is received by the ME from the network, the
                  ME shall modify the EF using the UPDATE command. This new PLMN shall be stored in the fourth position,
                  and the existing list 'shifted' causing the previous contents of the first position to be lost.
                  When less than four FPLMNs exist in the EF, storage of an additional FPLMN shall not cause any
                  existing FPLMN to be lost.",
                                    
LOCI          => "EF LOCI '6F7E' (Location information): This EF contains the following Location Information:\n
                 - Temporary Mobile Subscriber Identity (TMSI) \n - Location Area Information (LAI) \n
                 - TMSI TIME \n - Location update status.",

AD            => "EF AD '6FAD' (Administrative data): This EF contains information concerning the mode of operation
                  according to the type of SIM, such as normal (to be used by PLMN subscribers for GSM operations),
                  type approval (to allow specific use of the ME during type approval procedures of e.g. the radio
                  equipment), cell testing (to allow testing of a cell before commercial use of this cell), manufacturer
                  specific (to allow the ME manufacturer to perform specific proprietary auto-test in its ME during
                  e.g. maintenance phases).",

PHASE         => "EF Phase '6FAE' (Phase identification): This EF contains information concerning the phase of the SIM.",

VGCS          => "EF VGCS '6FB1' (Voice Group Call Service): This EF contains a list of those VGCS group identifiers the
                  user has subscribed to. The elementary file is used by the ME for group call establishment and group
                  call reception.",
                  
VGCSS         => "EF VGCSS '6FB2' (Voice Group Call Service Status): This EF contains the status of activation for the VGCS
                  group identifiers. The elementary file is directly related to the EFVGCS. This EF shall always be
                  allocated if EFVGCS is allocated.",
                  
VBS           => "EF VBS '6FB3' (Voice Broadcast Service): This EF contains a list of those VBS group identifiers the user
                  has subscribed to. The elementary file is used by the ME for broadcast call establishment and
                  broadcast call reception.",
                  
VBSS          => "EF VBSS '6FB4' (Voice Broadcast Service Status): This EF contains the status of activation for the VBS
                  group identifiers. The elementary file is directly related to the EFVBS. This EF shall always be
                  allocated if EFVBS is allocated.",

eMLPP         => "EF eMLPP '6FB5' (enhanced Multi Level Pre-emption and Priority): This EF contains information about priority
                  levels and fast call set-up conditions for the enhanced Multi Level Preemption and Priority service
                  that which can be used by the subscriber.",
                  
AAeM          => "EF AAeM '6FB6' (Automatic Answer for eMLPP Service): This EF contains those priority levels (of the Multi
                  Level Pre-emption and Priority service) for which the mobile station shall answer automatically to
                  incoming calls.",
                  
ECC           => "EF ECC '6FB7' (Emergency Call Codes): This EF contains up to 5 emergency call codes.",

CBMIR         => "EF CBMIR '6F50' (Cell broadcast message identifier range selection): This EF contains ranges of cell
                  broadcast message identifiers that the subscriber wishes the MS to accept. Any number of CB Message
                  Identifier Parameter ranges may be stored in the SIM. No order of priority is applicable.",
                  
NIA           => "EF NIA '6F51' (Network's Indication of Alerting): This EF contains categories and associated text related
                  to the Network's indication of alerting in the MS service defined in TS 02.07 [3].",
                  
KcGPRS        => "EF KcGPRS '6F52' (GPRS Ciphering key KcGPRS): This EF contains the ciphering key KcGPRS and the ciphering
                  key sequence number n for GPRS (see TS 23.060 [32]).",

LOCIGPRS      => "EF LOCIGPRS '6F53' (GPRS location information): This EF contains the following Location Information:\n
                  - Packet Temporary Mobile Subscriber Identity (P-TMSI);\n - Packet Temporary Mobile Subscriber
                  Identity signature value (P-TMSI signature value);\n - Routing Area Information (RAI);\n
                  - Routing Area update status.",
                  
SUME          => "EF SUME '6F54' (SetUpMenu Elements): This EF contains Simple TLVs related to the menu title to be used by a
                  SIM card supporting the SIM API when issuing a SET UP MENU proactive command.",
                  
PLMNwACT      => "EF PLMNwAcT '6F60' (User controlled PLMN Selector with Access Technology): This EF contains coding for
                  n PLMNs, where n is at least eight. This information, determined by the user, defines the
                  preferred PLMNs of the user in priority order. The EF also contains the Access Technologies for
                  each PLMN in this list. (see TS 23.122 [51]).",
                  
OPLMNwACT     => "EF OPLMNwAcT '6F61' (Operator controlled PLMN Selector with Access Technology): This EF contains coding
                  for n PLMNs, where n is at least eight. This information, determined by the operator, defines
                  the preferred PLMNs of the operator in priority order. The EF also contains the Access Technologies
                  for each PLMN in this list (see TS 23.122 [51]).",
                  
HPLMNwACT      => "EF HPLMNwAcT '6F62' (HPLMN Selector with Access Technology): The HPLMN Selector with access technology data
                  field shall contain the HPLMN code, or codes together with the respective access technology in
                  priority order (see TS 23.122 [51]).",
                  
CPBCCH        => "EF CPBCCH '6F63' (CPBCCH Information): This EF contains information concerning the CPBCCH according to
                  TS 04.18 [48] and TS 03.22 [45]. CPBCCH storage may reduce the extent of a Mobile Station's search
                  of CPBCCH carriers when selecting a cell. The CPBCCH carrier lists shall be in accordance with the
                  procedures specified in TS 04.18 [48], TS 04.60 [49] and TS 03.22 [45]. The MS stores CPBCCH
                  information from the System Information 19 message, Packet System Information 3, and Packet System
                  Information 3 bis on the SIM. The same CPBCCH carrier shall never occur twice in the list.",
                  
INVSCAN       => "EF InvScan '6F64' (Investigation Scan): This EF contains two flags used to control the investigation scan
                  for higher prioritized PLMNs not offering voice services.",

# EFs of DF '5F 3C'
MExEST        => "EF MExEST '4F40' (MExE Service Table): This EF indicates which MExE services are allocated, and whether, if allocated, the service is activated. If a service is
                  not allocated or not activated in the SIM, the ME shall not select this service.",

ORPK          => "EF OPRK '4F41' (Operator Root Public Key): This EF contains the descriptor(s ) of certificates containing the Operator Root Public Key. This EF shall only be
                  allocated if the operator wishes to verify applications and certificates in the MExE operator domain using a root public
                  key held on the SIM. Each record of this EF contains one certificate descriptor.",

ARPK          => "EF APRK '4F42' (Administrator Root Public Key): This EF contains the descriptor(s ) of certificates containing the Administrator Root Public Key. This EF shall only be
                  allocated if the SIM issuer wishes to control the Third Party certificates on the terminal using an Administrator Root
                  Public Key held on the SIM. Each record of this EF contains one certificate descriptor.",

TPRPK         => "EF TPRPK '4F43' (Third Party Root Public key): This EF contains descriptor(s ) of certificates containing the Third Party Root Public key (s). This EF shall only be
                  allocated if the SIM issuer wishes to verify applications and certificates in the MExE Third Party domain using root
                  public key(s) held on the SIM. This EF can contain one or more root public keys. Each record of this EF contains one
                  certificate descriptor.",
                  
# EFs of DF '5F 70'

SAI           => "EF SAI '4F30' (SoLSA Access Indicator): This EF contains the 'LSA only access indicator'. This EF shall always be allocated if DFSoLSA is present.",

SLL           => "EF SLL '4F31' (SoLSA LSA List): This EF contains information describing the LSAs that the user is subscribed to. This EF shall always be allocated if
                  DFSoLSA is present.",

# EFs of DF '7F 10'
ADN           => "EF ADN '6F3A' (Abbreviated dialling numbers): This EF contains Abbreviated Dialling Numbers (ADN) and/or
                  Supplementary Service Control strings (SSC). In addition it contains identifiers of associated
                  network/bearer capabilities and identifiers of extension records. It may also contain an associated
                  alpha-tagging.",
                  
FDN           => "EF FDN '6F3B' (Fixed dialling numbers): This EF contains Fixed Dialling Numbers (FDN) and/or Supplementary
                    Service Control strings (SSC). In addition it contains identifiers of associated network/bearer
                    capabilities and identifiers of extension records. It may also contain an associated alpha-tagging.",

SMS           => "EF SMS '6F3C' (Short messages): This EF contains information in accordance with TS 23.040 [13] comprising
                  short messages (and associated parameters) which have either been received by the MS from the network,
                  or are to be used as an MS originated message.",
                  
CCP           => "EF CCP '6F3D' (Capability configuration parameters): This EF contains parameters of required network and
                    bearer capabilities and ME configurations associated with a call established using an abbreviated
                    dialling number, a fixed dialling number, an MSISDN, a last number dialled, a service dialling
                    number or a barred dialling number.",
                    
MSISDN        => "EF MSISDN '6F40' (MSISDN): This EF contains MSISDN(s) related to the subscriber. In addition it contains
                    identifiers of associated network/bearer capabilities and identifiers of extension records.
                    It may also contain an associated alpha-tagging.",
                    
SMSP          => "EF SMSP '6F42' (Short message service parameters): This EF contains values for Short Message Service header
                    Parameters (SMSP), which can be used by the ME for user assistance in preparation of mobile
                    originated short messages. For example, a service centre address will often be common to many short
                    messages sent by the subscriber.",
                    
SMSS          => "EF SMSS '6F43' (SMS status): This EF contains status information relating to the short message service.
                    The provision of this EF is associated with EFSMS. Both files shall be present together, or both
                    absent from the SIM.",
                    
LND           => "EF LND '6F44' (Last number dialled): This EF contains the last numbers dialled (LND) and/or the respective
                  supplementary service control strings (SSC). In addition it contains identifiers of associated
                  network/bearer capabilities and identifiers of extension records. It may also contain associated
                  alpha-tagging.",
                  
SMSR          => "EF SMSR '6F47' (Short message status reports): This EF contains information in accordance with TS 23.040 [13]
                  comprising short message status reports which have been received by the MS from the network. Each
                  record is used to store the status report of a short message in a record of EFSMS. The first byte
                  of each record is the link between the status report and the corresponding short message in EFSMS.",
                  
SDN           => "EF SDN '6F49' (Service Dialling Numbers): This EF contains special service numbers (SDN) and/or the respective
                  supplementary service control strings (SSC). In addition it contains identifiers of associated
                  network/bearer capabilities and identifiers of extension records. It may also contain associated
                  alpha-tagging.",
                  
EXT1          => "EF EXT1 '6F4A' (Extension1): This EF contains extension data of an ADN/SSC, an MSISDN, or an LND. Extension
                    data is caused by:\n - an ADN/SSC (MSISDN, LND) which is greater than the 20 digit capacity of the
                    ADN/SSC (MSISDN, LND)\n Elementary File or where common digits are required to follow an ADN/SSC
                    string of less than 20 digits. The remainder is stored in this EF as a record, which is identified
                    by a specified identification byte inside the ADN/SSC (MSISDN, LND) Elementary File. The EXT1
                    record in this case is specified as additional data;\n - an associated called party subaddress.
                    The EXT1 record in this case is specified as subaddress data.",
                    
EXT2          => "EF EXT2 '6F4B' (Extension2): This EF contains extension data of an FDN/SSC (see EXT2 in subclause 10.5.2).",

EXT3          => "EF EXT3 '6F4C' (Extension3): This EF contains extension data of an SDN (see EXT3 in subclause 10.5.9).",

BDN           => "EF BDN '6F4D' (Barred Dialling Numbers): This EF contains Barred Dialling Numbers (BDN) and/or Supplementary
                  Service Control strings (SSC). In addition it contains identifiers of associated network/bearer
                  capabilities and identifiers of extension records. It may also contain an associated alpha-tagging.",
                  
EXT4          => "EF EXT4 '6F4E' (Extension4): This EF contains extension data of an BDN/SSC (see EXT4 in subclause 10.5.13).",
                   
);





##########################################################################################
### MAIN PROGRAM
##########################################################################################

my $simple   = XML::Simple->new();         #Initialize the object
my $file_in  = $ARGV[0];
my $file_tmp = "./tmp.xml";

my $argc = @ARGV;
if ($argc != 1){
        print "Usage: perl wrapper_simple.pl <image.xml> | e.g. perl wrapper_simple.pl image.xml\n";
        print "Parser which converts the raw (U)SIM image in text plain format\n";
        exit 1;
}


# A copy of the original file is made to preserve its integrity
copy("$file_in","$file_tmp") or die "Copy failed: $!";

# forcearray => 1 is used in order to have a list of elements 
my $tree = $simple->XMLin("$file_tmp", forcearray => 1);
open FIN, ">dump.txt";
print FIN Dumper($tree);
close FIN;

my $x=0;
## MF header parsing
my @mf = split(/ /,&trim($tree->{mf}->[0]->{header}->[0]));
#print "DEBUG header MF: @mf\n";
&Parse_Header_DF(\@mf, $standard_file2{"3F00"});

## parsing of EFs under MF
my $root_mf = $tree->{mf}->[0]->{ef};
#print "DEBUG: $root_mf\n";
while ($root_mf->[$x]){
    my @header = split(/ /,&trim($root_mf->[$x]->{header}->[0]));    
    #print "Header EF under MF: @header\n";
    $id_file = $header[4].$header[5];
    #print "DEBUG id_file: ".$id_file."\n";
    &Parse_Header_EF(\@header, $standard_file2{"$id_file"});
    my @body = split(/ /,&trim($root_mf->[$x]->{body}->[0]->{content}));
    #print "DEBUG body: @body\n";
    &Parse_MF00($id_file, \@body);
    $x++;
}


my $i=0; #counter for addressing all the DFs under the Master File
my $n_df=0; #current Df being parsed
#DF 7F20
#MF absolute path in the XML tree (XML::Simple)
my $root_mf1=$tree->{mf}->[0]->{df};

while ($root_mf1->[$i]){
    
    my @h = split(/ /, &trim($root_mf1->[$i]->{header}->[0]));
    #print "DEBUG array h: @h\n";
    $id_df = $h[4].$h[5];
    
    &Parse_Header_DF(\@h, $standard_file2{"$id_df"});
    #Parsing of the whole set of EFs and DFs under DF 7F20
    
    if ($id_df eq '7F20'){
        #base of the absolute path for DF 7F20
        my $root_7F20=$root_mf1->[$i]->{ef};
        my $n_ef=0;  #the number of elementary files under the Master File starts from zero
        #
        while ($root_7F20->[$n_ef]->{header}->[0]){
            
            my @header = split(/ /, &trim($root_7F20->[$n_ef]->{header}->[0]));
            #print "DEBUG array header: @header\n";
            $id_file = $header[4].$header[5];
            #print "ID__FILE ".($n_ef+1).": $id_file\n";
            &Parse_Header_EF(\@header, $standard_file2{"$id_file"});
           
            if ($header[13] eq '00'){ # if the EF is transparent
                my @body = split(/ /,&trim($root_7F20->[$n_ef]->{body}->[0]->{content}));
                #print "DEBUG EF transparent body: @body\n";
                &Parse_7F20($id_file, \@body);
            }
            else{ # if the structure is linear fixed or cyclic
                my $counter=0;
                if ($root_7F20->[$n_ef]->{body}->[0]->{record} =~ m/HASH/ ){ # Case of LF file with one record
                    my @body = split(/ /,&trim($root_7F20->[$n_ef]->{body}->[0]->{record}->[0]->{content}));
                    #print "DEBUG EF linear fixed body: @body\n";
                    &Parse_7F20($id_file, \@body, $counter);
                }
                else{ # Case of LF file with multiple records
                    while (defined($root_7F20->[$n_ef]->{body}->[0]->{record}->[$counter]->{content})){ 
                        my @body = split(/ /,&trim($root_7F20->[$n_ef]->{body}->[0]->{record}->[$counter]->{content}));
                        #print "DEBUG body Linear Fixed file rec $counter: body: @body\n";
                        &Parse_7F20($id_file, \@body, $counter);  
                        $counter++;                        
                    }
                }                
            }
            $n_ef++;
        }

    #    #looking for DF 5F3C
    #    my $y=0;
    #    #my $root_mf_df=$tree->{mf}->[0]->{df}->[$n_df];
    #    #$root_mf1=$tree->{mf}->[0]->{df};
    #    my $root_5F3C = $root_mf1->[$i]->{df};
    #    print "root mf df: $root_mf_df\n";
    #    #while ($root_mf_df->[$y]->{header}->[0]){
    #    if (defined($root_5F3C->[0]->{header}->[0])){
    #        my @h = split(/ /, &trim($root_mf_df->[$y]->{header}));
    #        $id_df=$h[4].$h[5];
    #        &Parse_Header_DF(\@h, $standard_file2{"$id_df"});
    #        if ($id_df eq '5F3C'){
    #            my $n_df_df=$y;
    #            my $n_ef=0;
    #            my $root_5F3C=$tree->{mf}->{df}->[$n_df]->{df}->[$n_df_df]->{ef};
    #            while ($root_5F3C->[$n_ef]->{header}){
    #                my @header = split(/ /, $root_5F3C->[$n_ef]->{header});
    #                $id_file = $header[4].$header[5];
    #                &Parse_Header_EF(\@header, $standard_file2{"$id_file"});
    #            
    #                if ($header[13] eq '00'){ # if the EF is transparent
    #                    my @body = split(/ /,$RE{ws}{crop}->subs($tree->{mf}->{df}->[$n_df]->{df}->[$n_df_df]->{ef}->[$n_ef]->{body}->{content}));
    #                    &Parse_5F3C($id_file, \@body);
    #                }
    #                else{ # On the contrary if the structure is linear fixed or cyclic
    #                    my $counter=0;
    #                    if ($root_5F3C->[$n_ef]->{body}->{record} =~ m/HASH/ ){ # Case of LF file with one record
    #                        my @body = split(/ /,$RE{ws}{crop}->subs($root_5F3C->[$n_ef]->{body}->{record}->{content}));
    #                        &Parse_5F3C($id_file, \@body, $counter);
    #                    }
    #                    else{ # Case of LF file with multiple records
    #                        while (defined($root_5F3C->[$n_ef]->{body}->{record}->[$counter])){ 
    #                            my @body = split(/ /,$RE{ws}{crop}->subs($root_5F3C->[$n_ef]->{body}->{record}->[$counter]->{content}));
    #                            &Parse_5F3C($id_file, \@body, $counter);  
    #                            $counter++;                        
    #                        }
    #                    }
    #                }
    #            $n_ef++;
    #            }                
    #        }
    #        elsif ($id_df eq '5F70'){
    #            my $n_df_df=$y;
    #            my $n_ef=0;
    #            my $root_5F70=$tree->{mf}->{df}->[$n_df]->{df}->[$n_df_df]->{ef};
    #            while ($root_5F70->[$n_ef]->{header}){
    #                my @header = split(/ /, $root_5F70->[$n_ef]->{header});
    #                $id_file = $header[4].$header[5];
    #                &Parse_Header_EF(\@header, $standard_file2{"$id_file"});
    #            
    #                if ($header[13] eq '00'){ # if the EF is transparent
    #                    my @body = split(/ /,$RE{ws}{crop}->subs($root_5F70->[$n_ef]->{body}->{content}));
    #                    #print "@body\n\n";
    #                    &Parse_5F70($id_file, \@body);
    #                }
    #                else{ # On the contrary if the structure is linear fixed or cyclic
    #                    my $counter=0;
    #                    if ($root_5F70->[$n_ef]->{body}->{record} =~ m/HASH/ ){ # Case of LF file with one record
    #                        my @body = split(/ /,$RE{ws}{crop}->subs($root_5F70->[$n_ef]->{body}->{record}->{content}));
    #                        &Parse_5F70($id_file, \@body, $counter);
    #                    }
    #                    else{ # Case of LF file with multiple records
    #                        while (defined($root_5F70->[$n_ef]->{body}->{record}->[$counter])){ 
    #                            my @body = split(/ /,$RE{ws}{crop}->subs($root_5F70->[$n_ef]->{body}->{record}->[$counter]->{content}));
    #                            &Parse_5F70($id_file, \@body, $counter);  
    #                            $counter++;                        
    #                        }
    #                    }
    #                }
    #            $n_ef++;
    #            }                
    #        }
    #    $y++;
    #    }
    #
    }##End routine which parses EFs and DFs under 7F20
    
    #looking for 7F10 under MF 3F00
  
    if ($id_df eq '7F10'){       
        my $n_ef=0;        
        my $root_7F10=$root_mf1->[$i]->{ef};
        while ($root_7F10->[$n_ef]->{header}->[0]){
           
            my @header = split(/ /, &trim($root_7F10->[$n_ef]->{header}->[0]));
            $id_file = $header[4].$header[5];
            #print "ID__FILE ".($n_ef).": $id_file\n";
            &Parse_Header_EF(\@header, $standard_file2{"$id_file"});
            
            if ($header[13] eq '00'){ # if the EF is transparent
                my @body = split(/ /,&trim($root_7F10->[$n_ef]->{body}->[0]->{content}));
                #print "DEBUG EF transparent: @body\n";
                &Parse_7F10($id_file, \@body);
            }
            else{ # On the contrary if the structure is linear fixed or cyclic
                my $counter=0;
                if ($root_7F10->[$n_ef]->{body}->[0]->{record} =~ m/HASH/ ){ # Case of LF file with one record
                    my @body = split(/ /,&trim($root_7F10->[$n_ef]->{body}->[0]->{record}->[0]->{content}));
                    #print "DEBUG linear fixed body: @body\n";                   
                    &Parse_7F10($id_file, \@body, $counter);
                }
                else{ # Case of LF file with multiple records
                    while (defined($root_7F10->[$n_ef]->{body}->[0]->{record}->[$counter]->{content})){                       
                        my @body = split(/ /,&trim($root_7F10->[$n_ef]->{body}->[0]->{record}->[$counter]->{content}));
                        #print "DEBUG body Linear Fixed file rec $counter: body: @body\n";
                        &Parse_7F10($id_file, \@body, $counter);  
                        $counter++;                        
                    }
                }                
            }            
            $n_ef++;
        }
    }
    
    $i++; #Next DF
    
}


###############################################################################################
sub Parse_MF00{

        $id_file        = shift(@_);
        my $ref_body    = shift(@_);
        my @body        = @$ref_body;
        
        if ($id_file eq '2FE2'){
            # ICCID
            &Parse_ICCID(@body);
        }
        elsif ($id_file eq '2F05'){
            # ELP
            &Parse_ELP(@body);
        }

}
################################################################################################
sub Parse_7F20 {
        
        $id_file        = shift(@_);
        my $ref_body    = shift(@_);
        my @body        = @$ref_body;
        my $counter     = shift(@_);
        
        if ($id_file eq '6F05'){
            # LP
            &Parse_LP(@body);
        }
        elsif ($id_file eq '6F07'){
            # IMSI        
            &Parse_IMSI(@body);
        }
        elsif ($id_file eq '6F20'){
            # Kc        
            &Parse_KC(@body);
        }
        elsif ($id_file eq '6F30'){
            # PLMNsel
            &Parse_PLMNsel(@body);
        }
        elsif ($id_file eq '6F31'){
            # HPLMN        
            &Parse_HPLMN(@body);
        }
        elsif ($id_file eq '6F37'){
            # ACMmax        
            &Parse_ACMmax(@body);
        }
        elsif ($id_file eq '6F38'){
            # SST
            &Parse_SST(@body);
        }
        elsif ($id_file eq '6F39'){
            # ACM
            &Parse_ACM(\@body, $counter);
        }
        elsif ($id_file eq '6F3E'){
            # GID1        
            &Parse_GID1(@body);
        }
        elsif ($id_file eq '6F3F'){
            # GID2        
            &Parse_GID2(@body);
        }
        elsif ($id_file eq '6F41'){
            # PUCT        
            &Parse_PUCT(@body);
        }
        elsif ($id_file eq '6F45'){
            # CBMI        
            &Parse_CBMI(@body);
        }
        elsif ($id_file eq '6F46'){
            # SPN        
            &Parse_SPN(@body);
        }
        elsif ($id_file eq '6F48'){
            # CBMID        
            &Parse_CBMID(@body);
        }
        elsif ($id_file eq '6F74'){
            # BCCH        
            &Parse_BCCH(@body);
        }
        elsif ($id_file eq '6F78'){
            # ACC        
            &Parse_ACC(@body);
        }
        elsif ($id_file eq '6F7B'){
            # FPLMN        
            &Parse_FPLMN(@body);
        }
        elsif ($id_file eq '6F7E'){
            # LOCI        
            &Parse_LOCI(@body);
        }
        elsif ($id_file eq '6FAD'){
            # AD        
            &Parse_AD(@body);
        }
        elsif ($id_file eq '6FAE'){
            # PHASE
            &Parse_Phase(@body);
        }
        elsif ($id_file eq '6FB1'){
            # VGCS        
            &Parse_VGCS(@body);
        }
        elsif ($id_file eq '6FB2'){
            # VGCSS        
            &Parse_VGCSS(@body);
        }
        elsif ($id_file eq '6FB3'){
            # VBS        
            &Parse_VBS(@body);
        }
        elsif ($id_file eq '6FB4'){
            # VBSS        
            &Parse_VBSS(@body);
        }
        elsif ($id_file eq '6FB5'){
            # eMLPP        
            &Parse_eMLPP(@body);
        }
        elsif ($id_file eq '6FB6'){
            # AAeM        
            &Parse_AAeM(@body);
        }   
        elsif ($id_file eq '6FB7'){
            # ECC
            &Parse_ECC(@body);
        }
        elsif ($id_file eq '6F50'){
            # CBMIR       
            &Parse_CBMIR(@body);
        }
        elsif ($id_file eq '6F51'){
            # NIA        
            &Parse_NIA(\@body, $counter);
        }
        elsif ($id_file eq '6F52'){
            # KcGPRS        
            &Parse_KcGPRS(@body);
        }
        elsif ($id_file eq '6F53'){
            # LOCIGPRS        
            &Parse_LOCIGPRS(@body);
        }
        elsif ($id_file eq '6F54'){
            # SUME        
            &Parse_SUME(@body);
        }
        elsif ($id_file eq '6F60'){
            # PLMNwACT       
            &Parse_PLMNwACT(@body);
        }
        elsif ($id_file eq '6F61'){
            # OPLMNwACT       
            &Parse_OPLMNwACT(@body);
        }
        elsif ($id_file eq '6F62'){
            # HPLMNwACT       
            &Parse_HPLMNwACT(@body);
        }
        elsif ($id_file eq '6F63'){
            # CPBCCH       
            &Parse_CPBCCH(@body);
        }
        elsif ($id_file eq '6F64'){
            # INVSCAN        
            &Parse_INVSCAN(@body);
        }
}    

################################################################################################
sub Parse_5F3C{
    
    $id_file        = shift(@_);    
    my $ref_body    = shift(@_);
    my @body        = @$ref_body;
    my $counter     = shift(@_);
                
        if ($id_file eq '4F40'){
            #MExEST
            &Parse_MExEST(@body);
        }
        elsif ($id_file eq '4F41'){
            #ORPK
            &Parse_ORPK(\@body, $counter);
        }
        elsif ($id_file eq '4F42'){
            #ARPK
            &Parse_ARPK(\@body, $counter);
        }
        elsif ($id_file eq '4F43'){
            #TPRPK
            &Parse_TPRPK(\@body, $counter);
        }
    }
################################################################################################
sub Parse_5F70{
    
    $id_file     = shift(@_);    
    my $ref_body    = shift(@_);
    my @body        = @$ref_body;
    my $counter     = shift(@_);

        if ($id_file eq '4F30'){
            #SAI
            &Parse_SAI(@body);
        }
        elsif ($id_file eq '4F31'){
            #SLL
            &Parse_SLL(\@body, $counter);
        }
    }
###############################################################################################
sub Parse_7F10 {
        
    $id_file     = shift(@_);    
    my $ref_body    = shift(@_);
    my @body        = @$ref_body;
    my $counter     = shift(@_);     
    
    if ($id_file eq '6F3A'){
        # ADN
        &Parse_ADN(\@body,$counter);
    }
    elsif ($id_file eq '6F3B'){
        # FDN
        &Parse_FDN(\@body,$counter);
    }
    elsif ($id_file eq '6F3C'){
        # SMS
        &Parse_SMS(\@body,$counter);
    }
    elsif ($id_file eq '6F3D'){
        # CCP        
        &Parse_CCP(\@body,$counter);
    }
    elsif ($id_file eq '6F40'){
        #MSISDN
        &Parse_MSISDN(\@body,$counter);
    }
    elsif ($id_file eq '6F42'){
        # SMSP 
        &Parse_SMSP(\@body,$counter);
        }
    elsif ($id_file eq '6F43'){
        # SMSS
        &Parse_SMSS(@body);
    }
    elsif ($id_file eq '6F44'){
        # LND        
        &Parse_LND(\@body,$counter);
    }
    elsif ($id_file eq '6F47'){
        # SMSR        
        &Parse_SMSR(\@body,$counter);
    }
    elsif ($id_file eq '6F49'){
        # SDN      
        &Parse_SDN(\@body,$counter);
    }
    elsif ($id_file eq '6F4A'){
        # EXT1        
        &Parse_EXT1(\@body,$counter);
    }
    elsif ($id_file eq '6F4B'){
        # EXT2       
        &Parse_EXT2(\@body,$counter);
    }
    elsif ($id_file eq '6F4C'){
        # EXT3        
        &Parse_EXT3(\@body,$counter);
    }
    elsif ($id_file eq '6F4D'){
        # BDN   
        &Parse_BDN(\@body,$counter);
    }
    elsif ($id_file eq '6F4E'){
        # EXT4        
        &Parse_EXT4(\@body,$counter);
        }
    }
##########################################################################################
### START PARSE HEADER_DF PROCEDURE 
##########################################################################################

##########################################################################################
### START PARSE HEADER_DF PROCEDURE 
##########################################################################################

sub Parse_Header_DF {
    my $ref = shift(@_);
    my @h = @$ref;
    my $ef_name = shift(@_);
    
    # (1-2) RFU 
    my $rfudf = $hd_df{'1-2'}.chop($h[0])." ".$h[1];
    
    # (3-4) Total amount of memory of the selected directory which is
    # not allocated to any of the DFs or EFs under the selected directory
    # Hexadecimal to Decimal conversion
    my $mmfdf = hex($h[2].$h[3]);
    
    # (5-6) File ID 
    my $iddf = $h[4].$h[5];
    
    # (7) Type of file 
    my $typedf = $file_type_df{$h[6]};
    
    # (8-12) RFU
    my $rfudf2 = $hd_df{'8-12'}.$h[7].$h[8].$h[9].$h[10].$h[11];
    
    # (13) Length of the following data (byte 14 to end)
    my $ldf =  hex($h[12]);
    
    # (14) File characteristics
    my @bin = split(//,&Hex2Bin($h[13]));
    # b134 = Clock Stop
    # b2   = 13/8 MHz (if eq 1) or 13/4 MHz (if eq 0)
    # b5   = "5V only SIM" (if eq 0) or "3V technology SIM" (if eq 1)
    # b67  = RFU
    # b8   = CHV1 enabled (if eq 0) or CHV1 disabled (if eq 1)
    my $cardf =
    $file_c{b134}{$bin[4].$bin[5].$bin[7]}."; ". 
    $file_c{b2}{$bin[6]}."; ".
    $file_c{b5}{$bin[3]}."; ".
    $file_c{b8}{$bin[0]};
    
    
    # (15) N° of DFs which are direct child of the current directory
    my $ndf = hex($h[14]);
    
    # (16) N° of EFs which are direct child of the current directory
    my $nef = hex($h[15]);
    
    # (17) N° of CHVs, UNBLOCK CHVs and administrative codes
    my $nch = hex($h[16]);
    
    # (18) RFU
    #print $hd_df{'18'}.$h[17]."\n\t";
    
    # (19) CHV1 status: pag. 42 standard ETSI TS 100 977; the first nibble (4 LSB) rapresents the remaining attempt 
    my $chs = hex(chop($h[18]))." attempt left";
    
    # (20) UNBLOCK CHV1 status: b8=0 CHV1 not initialized; b8=1 CHV1 initialized
    $t = $h[19] && "80";
    if($t eq "80"){
        $uch = "CHV1 initilized";
    } else {
        $uch = "CHV1 not initialized";
    }
    
    # (21) CHV2 status: pag. 42 standard ETSI TS 100 977; the first nibble (4 LSB) rapresents the remaining attempt
    my $chs2 = hex(chop($h[20]))." attempt left";
    
    # (22) UNBLOCK CHV2 status: b8=0 CHV2 not initialized; b8=1 CHV2 initialized
    $t = $h[21] && "80";
    if($t eq "80"){
        $uch2 = "CHV2 initilized";
    } else {
        $uch2 = "CHV2 not initialized";
    }
    
    @df_header = ();
           
    $df_header{"$ef_name"}={        
                    'ID'                    => [$iddf],
                    'FREE_MEMORY'           => [$mmfdf],
                    'TYPE'                  => [$typedf],
                    'LENGTH'                => [$ldf],
                    'File_characteristics'  => [$cardf],
                    'Number_of_DFs'         => [$ndf],         
                    'Number_of_EFs'         => [$nef],        
                    'Number_of_CHVs'        => [$nch],      
                    'CHV1_status'           => [$chs],    
                    'UNBLOCK_CHV1_status'   => [$uch],
                    'CHV2_status'           => [$chs2],  
                    'UNBLOCK_CHV2_status'   => [$uch2],             
                };

    return \@df_header;
    
}

##########################################################################################
### END PARSE HEADER_DF
##########################################################################################

##########################################################################################
### START PARSE HEADER_EF PROCEDURE 
##########################################################################################

sub Parse_Header_EF {
    
    my $ref = shift(@_);
    my @h = @$ref;    
    $ef_name = shift(@_);
    
    # (1-2) RFU 
    my $rfu = ($hd_ef{'1-2'}.chop($h[0]).$h[1]);
    
    # (3-4) File Size (Hexadecimal -> Decimal conversion)
    # for transparent EF: the length of the body part of the EF
    # for linear fixed or cyclic EF: record length multiplied by the
    # number of records of the EF
    my $filesize = hex($h[2].$h[3]);
    
    # (5-6) File ID 
    my $fileid = $h[4].$h[5];
    
    # (7) Type of file 
    # my $tof = $hd_ef{'7'}.$file_type_df{$h[6]};
    my $tof = $file_type_df{$h[6]};
    
    # (8) For transparent and linear fixed EFs this byte is RFU.
    # For a cyclic EF all bits except bit 7 are RFU
    # b7=1 indicates that the INCREASE command is allowed on the second cyclic file.
    # NOT USED
    #my $o = $hd_ef{'8'};
    #my $t = $file_struct{$h[13]}; # struct of file
    #if($t eq "transparent" || $t eq "linear fixed"){
    #  #my $o1='RFU';
    #} else {
    #  #$o1='RFU for cyclic file...';
    #}
    #my $of=$s.$o.$s.$o1.$s;
    
    # (9-11) Access Condition
    my $access = $hd_ef{'9-11'}.$h[8].$h[9].$h[10];
    # $h[8] with reference to etsi 100 977 pg 50
    # $sp[0] = UPDATE
    # $sp[1] = READ; SEEK
    # N.B: $sp[0] = LSD Least Significant Digit
    #      $sp[1] = MSD Most Significant Digit
    my @sp = split(//,$h[8]);    
    my $access1 = $ac{$sp[1]}; # UPDATE
    my $access2 = $ac{$sp[0]}; # READ; SEEK
    
    @sp = split(//,$h[9]); 
    my $access3 ='RFU: '.$ac{$sp[1]};   # RFU
    my $access4 = $ac{$sp[0]};          # INCREASE
    
    @sp = split(//,$h[10]); 
    my $access5 = $ac{$sp[1]};  # INVALIDATE
    my $access6 = $ac{$sp[0]};  # REHABILITATE
    
    # (12) File Status    
    @sp = split(//,Hex2Bin($h[11]));
    my $fs1 = "";
    my $fs2 = "";    
    if($sp[0] eq '0'){ # b1
      $fs1 = 'File invalidated';
    } else {
      $fs1 = 'File not invalidated';
    }  
    if($sp[2] eq '0'){ # b3 = 0; See ETSI 100 977 pg. 49
      $fs2 = 'File not readable or updatable when invalidated';
    } else { # b3 = 1;
      $fs2 = 'FIle readable and updatable when invalidated';
    }    
    my $fsf = $fs1."; ".$fs2;
    
    # (13) Length of the following data
    my $length = $hd_ef{'13'}.hex($h[12]);
    
    # (14) Structure of file
    my $sof = $file_struct{$h[13]};
    
    # (15) Length of a record
    my $rec = $hd_ef{'15'}.hex($h[14]);
    
    # (16..) RFU
    my $rfub = $hd_ef{'16'}.$h[15];
    @ef_header = ();
           
    $ef_header{"$ef_name"}={
                    'ID'                => [$fileid],             
                    'SIZE'              => [$filesize],         
                    'acREAD'            => [$access2],        
                    'acUPDATE'          => [$access1],      
                    'acINCREASE'        => [$access4],    
                    'acREHABILITATE'    => [$access6],
                    'acINVALIDATE'      => [$access5],  
                    'status'            => [$fsf],            
                    'structure'         => [$sof],         
                };
    
    
    
    return \@ef_header;		
}
##########################################################################################
### END PARSE HEADER_EF
##########################################################################################

################################################################################################
sub Parse_ICCID { 
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.52
    #This EF provides a unique identification number for the SIM.
    
    my @h = @_;
    my %nations = ( '39' => "Italy",
                    '43' => "Austria",
                    '32' => "Belgium",
                    '45' => "Denmark",
                    '33' => "France",
                    '49' => "Germany",
                    '44' => "England",
                    '30' => "Greece",
                  );
    my $out="";
    my $i=0;
    foreach my $k (@h){
        my $k = $h[$i];
        my @t = split(//,$k); 
        $out = $out.$t[1].$t[0]." ";
        if ($i == 1){ #byte 2 contains the nation
            $naz = $nations{$t[1].$t[0]};
        }
        $i++;
    }  
    return $iccid = $out."###".$naz;
}
################################################################################################
sub Parse_ELP {   #manca la norvegia
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.53
    #This EF contains the codes for up to n languages. This information, determined by the user/operator, defines the
    #preferred languages of the user in order of priority. This information may be used by the ME for MMI purposes.

    my @h = @_;
    my $out;
 
    my %language = ('de' => "German",
                    'en' => "English",
                    'it' => "Italian",
                    'fr' => "French",
                    'es' => "Spanish",
                    'nl' => "Dutch",
                    'se' => "Swedish",
                    'dk' => "Danish",
                    'pt' => "Portuguese",
                    '09' => "Norwegian",
                    'el' => "Greek",
                    'uk' => "Turkish",
                    'hu' => "Hungarian",
                    'pl' => "Polish",);
    $elp="";
    my $i=0;
    my @firstbit = " ";
    my @secondbit = " ";
    my $a = 1;
    for ($i=0;$i<scalar($#h);$i=$i+2){
        @firstbit = split(//,Hex2Bin($h[$i]));
        @secondbit = split(//,Hex2Bin($h[$i+1]));
        my $lan = $language{Trad2Sms(join('',$firstbit[1],$firstbit[2],$firstbit[3],$firstbit[4],$firstbit[5],$firstbit[6],$firstbit[7])).
            Trad2Sms(join('',$secondbit[1],$secondbit[2],$secondbit[3],$secondbit[4],$secondbit[5],$secondbit[6],$secondbit[7]))};
            $out="Language ".$a." is ".$lan."###";
        $elp=$elp.$out;
        $a++;
    }
    return $elp;
}

################################################################################################
sub Parse_LP {
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.54
    #This EF contains the codes for one or more languages. This information, determined by the user/operator, defines the
    #preferred languages of the user in order of priority. This information may be used by the ME for MMI purposes.
    
    #correspondence between byte->language
    my %language = ('00' => "German",
                    '01' => "English",
                    '02' => "Italian",
                    '03' => "French",
                    '04' => "Spanish",
                    '05' => "Dutch",
                    '06' => "Swedish",
                    '07' => "Danish",
                    '08' => "Portuguese",
                    '09' => "Norwegian",
                    '11' => "Greek",
                    '12' => "Turkish",
                    '13' => "Hungarian",
                    '14' => "Polish",
                    'FF' => "Language unspecified");
    
    my @h = @_;
    my $i=0; #byte position
    $lp=" ";
    for ($i=0;$i<=scalar($#h);$i++){
        $lp=$lp."Language ".($i+1)." is ".$language{$h[$i]}."###";
    }
    return $lp;
 }
################################################################################################
 sub Parse_IMSI {
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.54
    #This EF contains the International Mobile Subscriber Identity (IMSI).
    
    my @h = @_;
    my @out = split(//,$h[1].$h[2].$h[3].$h[4].$h[5].$h[6].$h[7].$h[8]);
    my $naz = $out[0].$out[3].$out[2].'F'.$out[5].$out[4];
    return $imsi=$out[1].$out[0].$out[3].$out[2].$out[5].$out[4].$out[7].$out[6].
    $out[9].$out[8].$out[11].$out[10].$out[13].$out[12].$out[15].$out[14]."###".$mnc{$naz};
 }
################################################################################################
sub Parse_KC {
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.55
    #This EF contains the ciphering key Kc and the ciphering key sequence number n.
    
    my @h = @_;     
    my $s = " ";    
    my $out = $h[0].$s.$h[1].$s.$h[2].$s.$h[3].$s.$h[4].$s.$h[5].$s.$h[6].$s.$h[7];    
    $kc = $out;    
    if ($h[8] eq  '07') { # particular case
        $kc = "Key not available!"; 
    }
    return $kc;
  }  
################################################################################################
sub Parse_PLMNsel {
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.56
    #This EF contains the coding for n PLMNs, where n is at least eight. This information determined by the user/operator
    #defines the preferred PLMNs of the user in priority order.
    
    my @h = @_;
    
    my $n = int((scalar($#h))/3); #each PLMNsel is composed of 3 bytes where n is number of PLMNsel
    my $out = "";
    $plmnsel = "";
    
    for (my $i=0;$i<$n;$i++){
        my $a = $i*3; #numeration bytes of PLMNsel
        my @plmn1 = split(//,$h[$a]);
        my @plmn2 = split(//,$h[$a+1]);
        my @plmn3 = split(//,$h[$a+2]);
        $out = ($i+1)." PLMN: ".$mnc{$plmn1[1].$plmn1[0].$plmn2[1].$plmn2[0].$plmn3[1].$plmn3[0]};
        $plmnsel .= $out."###";
    }
    #print "PLMNs: $plmnsel\n";
    return $plmnsel;
}
################################################################################################
sub Parse_HPLMN {
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.56
    #This EF contains the interval of time between searches for a higher priority PLMN (see TS 22.011 [5]).
    
    my @h = @_;
    my @t0 = split (//,Hex2Bin($h[0]));    
    my $out=16*$t0[0]+$t0[1]; #convert 2 bit in decimal numeration
    $hplmn="The time interval between two searches ".$out." minutes";
    return $hplmn;    
}
################################################################################################
sub Parse_ACMmax { 
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.57
    #This EF contains the maximum value of the accumulated call meter. This EF shall always be allocated if EFACM is
    #allocated.
    
    my @h = @_;
    my @t0 = @h;    
    my @t1 = split(//,Hex2Bin($t0[0])); 
    my @t2 = split(//,Hex2Bin($t0[1]));
    my @t3 = split(//,Hex2Bin($t0[2]));        
    my $acmmax0 = "ACMmax: ";    
    my $i;
    my $k;
    my $totale1=0;
    my $totale2=0;
    my $totale3=0;    
    for ($i=0;$i<8;$i++){ #convert bit in acmmax value
       $totale1=$totale1+(2**(23-$i))*($t1[$i]);  
       $totale2=$totale2+(2**(15-$i))*($t2[$i]);
       $totale3=$totale3+(2**(7-$i))*($t3[$i]);
   }
   my $acmmax1 = $totale1+$totale2+$totale3;
   $acmmax = $acmmax0.$acmmax1;
   return $acmmax;  
 } 
################################################################################################
sub Parse_SST {    #far apparire i servizi anche se non "allocati" nella SIM??
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.58
    #This EF indicates which services are allocated, and whether, if allocated, the service is activated. If a service is not
    #allocated or not activated in the SIM, the ME shall not select this service.
    
    my @h = @_;
    
    my %results = ('00' =>" not allocated",
                   '01' =>" not allocated",
                   '10' =>" allocated but not activated",
                   '11' =>" allocated and activated",);
    
    my @out1 = split(//,Hex2Bin($h[0]));
    my @out2 = split(//,Hex2Bin($h[1]));
    my @out3 = split(//,Hex2Bin($h[2]));
    my @out4 = split(//,Hex2Bin($h[3]));
    my @out5 = split(//,Hex2Bin($h[4]));
    my @out6 = split(//,Hex2Bin($h[5]));
    my @out7 = split(//,Hex2Bin($h[6]));
    my @out8 = split(//,Hex2Bin($h[7]));
    my @out9 = split(//,Hex2Bin($h[8]));
    my @out10 = split(//,Hex2Bin($h[9]));
    my @out11 = split(//,Hex2Bin($h[10]));
    my @out12 = split(//,Hex2Bin($h[11]));
    #my @out13 = split(//,Hex2Bin($h[12]));
    
    return $sst= "Service 1 : CHV1 disable function".$results{$out1[7].$out1[6]}."###".
    "Service 2 : Abbreviated Dialling Numbers (ADN)".$results{$out1[5].$out1[4]}."###".
    "Service 3 : Fixed Dialling Numbers (FDN)".$results{$out1[3].$out1[2]}."###".
    "Service 4 : Short Message Storage (SMS)".$results{$out1[1].$out1[0]}."###".
    "Service 5 : Advice of Charge (AoC)".$results{$out2[7].$out2[6]}."###".
    "Service 6 : Capability Configuration Parameters (CCP)".$results{$out2[5].$out2[4]}."###".
    "Service 7 : PLMN selector".$results{$out2[3].$out2[2]}."###".
    "Service 8 : RFU".$results{$out2[1].$out2[0]}."###".
    "Service 9 : MSISDN".$results{$out3[7].$out3[6]}."###".
    "Service 10: Extension1".$results{$out3[5].$out3[4]}."###".
    "Service 11: Extension2".$results{$out3[3].$out3[2]}."###".
    "Service 12: SMS Parameters".$results{$out3[1].$out3[0]}."###".
    "Service 13: Last Number Dialled (LND)".$results{$out4[7].$out[6]}."###".
    "Service 14: Cell Broadcast Message Identifier".$results{$out4[5].$out[4]}."###".
    "Service 15: Group Identifier Level 1".$results{$out4[3].$out[2]}."###".
    "Service 16: Group Identifier Level 2".$results{$out4[1].$out[0]}."###".
    "Service 17: Service Provider Name".$results{$out5[7].$out5[6]}."###".
    "Service 18: Service Dialling Numbers (SDN)".$results{$out5[5].$out5[4]}."###".
    "Service 19: Extension3".$results{$out5[3].$out5[2]}."###".
    "Service 20: RFU".$results{$out5[1].$out5[0]}."###".
    "Service 21: VGCS Group Identifier List (EFVGCS and EFVGCSS)".$results{$out6[7].$out6[6]}."###".
    "Service 22: VBS Group Identifier List (EFVBS and EFVBSS)".$results{$out6[5].$out6[4]}."###".
    "Service 23: enhanced Multi-Level Precedence and Pre-emption Service".$results{$out6[3].$out6[2]}."###".
    "Service 24: Automatic Answer for eMLPP".$results{$out6[1].$out6[0]}."###".
    "Service 25: Data download via SMS-CB".$results{$out7[7].$out7[6]}."###".
    "Service 26: Data download via SMS-PP".$results{$out7[5].$out7[4]}."###".
    "Service 27: Menu selection".$results{$out7[3].$out7[2]}."###".
    "Service 28: Call control".$results{$out7[1].$out7[0]}."###".
    "Service 29: Proactive SIM".$results{$out8[7].$out8[6]}."###".
    "Service 30: Cell Broadcast Message Identifier Ranges".$results{$out8[5].$out8[4]}."###".
    "Service 31: Barred Dialling Numbers (BDN)".$results{$out8[3].$out8[2]}."###".
    "Service 32: Extension4".$results{$out8[1].$out8[0]}."###".
    "Service 33: De-personalization Control Keys".$results{$out9[7].$out9[6]}."###".
    "Service 34: Co-operative Network List".$results{$out9[5].$out9[4]}."###".
    "Service 35: Short Message Status Reports".$results{$out9[3].$out9[2]}."###".
    "Service 36: Network's indication of alerting in the MS".$results{$out9[1].$out9[0]}."###".
    "Service 37: Mobile Originated Short Message control by SIM".$results{$out10[7].$out10[6]}."###".
    "Service 38: GPRS".$results{$out10[5].$out10[4]}."###".
    "Service 39: Image (IMG)".$results{$out10[3].$out10[2]}."###".
    "Service 40: SoLSA (Support of Local Service Area)".$results{$out10[1].$out10[0]}."###".
    "Service 41: USSD string data object supported in Call Control".$results{$out11[7].$out11[6]}."###".
    "Service 42: RUN AT COMMAND command".$results{$out11[5].$out11[4]}."###".
    "Service 43: User controlled PLMN Selector with Access Technology".$results{$out11[3].$out11[2]}."###".
    "Service 44: Operator controlled PLMN Selector with Access Technology".$results{$out11[1].$out11[0]}."###".
    "Service 45: HPLMN Selector with Access Technology".$results{$out12[7].$out12[6]}."###".
    "Service 46: CPBCCH Information".$results{$out12[5].$out12[4]}."###".
    "Service 47: Investigation Scan".$results{$out12[3].$out12[2]}."###".
    "Service 48: Extended Capability Configuration Parameters".$results{$out12[1].$out12[0]}."###";
    #"Service 49: MExe".$results{$out13[7].$out13[6]}."###".
    #"Service 50: RFU".$results{$out13[5].$out13[4]}."###".
    #"Service 51: RFU".$results{$out13[3].$out13[2]}."###".
    #"Service 52: RFU".$results{$out13[1].$out13[0]}."###";
}
################################################################################################
sub Parse_ACM{
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.60
    #This EF contains the total number of units for both the current call and the preceding calls.
    
    my $ref = shift(@_);
    my @h = @$ref;
    my $counter = shift(@_); #this EF has file_struct linear fixed
    
    my @t0 = @h;
    my @t1 = split(//,Hex2Bin($t0[0]));
    my @t2 = split(//,Hex2Bin($t0[1]));
    my @t3 = split(//,Hex2Bin($t0[2]));    
    my $acm0 = "ACM: ";
    my $i;
    my $totale1=0;
    my $totale2=0;
    my $totale3=0;    
    for ($i=0;$i<8;$i++){ #convert bit in acm value
       $totale1=$totale1+(2**(23-$i))*($t1[$i]);  
       $totale2=$totale2+(2**(15-$i))*($t2[$i]);
       $totale3=$totale3+(2**(7-$i))*($t3[$i]);
   }
   my $acm1 = $totale1+$totale2+$totale3;   
   return $acm[$counter] = $acm0.$acm1; 
}
################################################################################################
sub Parse_GID1 {
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.61
    #This EF contains identifiers for particular SIM-ME associations. It can be used to identify a group of SIMs for a
    #particular application.
    
    my @h = @_;
    my $out=join(" ",@h);
    return $gid1 = $out;
}
################################################################################################
sub Parse_GID2 {
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.61
    #This EF contains identifiers for particular SIM-ME associations. It can be used to identify a group of SIMs for a
    #particular application.
    
    my @h = @_;
    my $out = join(" ",@h);
    return $gid2 = $out;
}
################################################################################################
sub Parse_PUCT  { 
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.62
    #This EF contains the Price per Unit and Currency Table (PUCT). The PUCT is Advice of Charge related information
    #which may be used by the ME in conjunction with EFACM to compute the cost of calls in the currency chosen by the
    #subscriber, as specified in TS 22.024 [7]. This EF shall always be allocated if EFACM is allocated.
    
    my @h = @_;
    my @t1 = split(//,Hex2Bin($h[3]));
    my @t2 = split(//,Hex2Bin($h[4]));
    #EPPU Elementary Price per Unit
    my $eppu=(1*$t2[7]+2*$t2[6]+4*$t2[5]+8*$t2[4]+16*$t1[7]+32*$t1[6]+64*$t1[5]+128*$t1[4]+256*$t1[3]+512*$t1[2]+1024*$t1[1]+2048*$t1[0]);
    #EX the absolute value of its decimal logarithm
    my $ex=1*$t2[2]+2*$t2[1]+4*$t2[0];
    #price per unit = EPPU * 10EX
    my $price = $eppu * 10**$ex;    
    $puct = "EPPU(Elementary Price Per Unit): ".$eppu."###"."Price per unit = EPPU * 10^EX = ".$price."###";
    return $puct;
} 
################################################################################################
sub Parse_CBMI {
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.63
    #This EF contains the Message Identifier Parameters which specify the type of content of the cell broadcast messages
    #that the subscriber wishes the MS to accept.
    
    my @h = @_;
    my $result = " ";
    #as in TS 23.041, "Message Format on BTS-MS Interface - Message Identifier".
    #Values listed show the types of message which shall be accepted by the MS.
    #Unused entries shall be set to 'FF FF'.
    for (my $i=0;$i<=(scalar($#h)/2);$i++){
        
        if(($h[($i*2)].$h[$i*2+1]) == 'FFFF'){
            $result = "Empty";
        }else{
            $result = $h[($i*2)].$h[$i*2+1];
        };
        $cbmi=$cbmi."CB message Identifier ".($i+1)." ".$result."###";
    };
    return $cbmi; #Cell Broadcast Message Identifier
}    
################################################################################################
sub Parse_SPN {
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.61
    #This EF contains the service provider name and appropriate requirements for the display by the ME.
    
    my @h = @_;  
    my $spn0 = "Display condition: ";
    my @dc = split(//,Hex2Bin($h[0])); #@dc display condition
    my $spn1;
    if ($dc[7] eq 0){
        $spn1 = "PLMN not required!";
    }
    if ($dc[7] eq 1){
        $spn1 = "PLMN required!";
    }    
    my $spn2 = "Service Provider Name: ";    
    my $i; 
    my @fin;
    my $spn3;
    for ($i=1;$i<17;$i++){ 
        @fin = split(//,Hex2Bin($h[$i]));
        $spn3 = $spn3."\n".Trad2Sms(join('',$fin[1],$fin[2],$fin[3],$fin[4],$fin[5],$fin[6],$fin[7])); #text
    }
    $spn = $spn0.$spn1."###".$spn2.$spn3."###";
    return $spn; #Service Provider Name
}
################################################################################################
sub Parse_CBMID{
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.75
    #This EF contains the message identifier parameters which specify the type of content of the cell broadcast messages
    #which are to be passed to the SIM.
    
    my @h = @_;
    my $i;
    #as in TS 23.041 [14]. Values listed show the identifiers of messages which shall be accepted by the MS
    #to be passed to the SIM.
    my $cb;
    for ($i=0;$i<scalar($#h);$i++){
        if ($h[$i] eq 'FF'){
            $cb = "Empty";
        }
        else{
            $cb = $h[$i];
        }
        my $z=$i+1;
        $cbmid=$cbmid."CB Message Identifier".$z.": ".$cb."###";
        
    }
    return $cbmid; #Cell Broadcast Message Identifier for Data Download
}
################################################################################################
sub Parse_BCCH {  
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.64
    #This EF contains information concerning the BCCH according to TS 04.08 [15].
    
    my @h = @_;
    my $bc = join(" ",@h);
    #The information is coded as octets 2-17 of the "neighbour cells description information element" in
    #TS 04.08 [15].
    $bcch = $bc;
    return $bcch;
}
################################################################################################
sub Parse_ACC {
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.64
    #This EF contains the assigned access control class(es). TS 22.011 [5] refers. The access control class is a parameter to
    #control the RACH utilization. 15 classes are split into 10 classes randomly allocated to normal subscribers and 5 classes
    #allocated to specific high priority users. For more information see TS 22.011 [5].
    
    my @h = @_;
    my @byte1 = split(//,Hex2Bin($h[0]));
    my @byte2 = split(//,Hex2Bin($h[1]));
    #Each ACC is coded on one bit. An ACC is "allocated" if the corresponding bit is set to 1 and "not
    #allocated" if this bit is set to 0. Bit b3 of byte 1 is set to 0.
    my $acc0 = "Number of the ACC: ";
    my $i;
    for ($i=1;$i<9;$i++){
        if ($byte1[$i]==1){
            $acc = $acc0.$i;
        } 
        if ($byte2[$i]==1){
            $acc1 = $i + 8;
            $acc = $acc0.$acc1;
        }   
    }
    return $acc; #Access control class
}
################################################################################################
sub Parse_FPLMN{
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.65
    #This EF contains the coding for four Forbidden PLMNs (FPLMN). It is read by the ME as part of the SIM initialization
    #procedure and indicates PLMNs which the MS shall not automatically attempt to access.
    
    my @h = @_;    
    my $n = (scalar($#h))/3;#each FPLMN is composed from 3 bytes, n is number of FPLMN
    my $i;    
    for ($i=0;$i<$n;$i++){
        my $a=$i*'3';
        my @plmn1 = split(//,$h[$a]);
        my @plmn2 = split(//,$h[$a+1]);
        my @plmn3 = split(//,$h[$a+2]);
        my $out = ($i+1)." PLMN: ".$mnc{$plmn1[1].$plmn1[0].$plmn2[1].$plmn2[0].$plmn3[1].$plmn3[0]};
        $fplmn=$fplmn.$out."###";
    }
    return $fplmn;
}
################################################################################################
sub Parse_LOCI {#strutturare tipo tag
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.66
    #This EF contains the following Location Information.
    
    my @h = @_;
    
    my $loci0 = "Temporary Mobile Subscriber Identity TMSI: ".$h[0].$h[1].$h[2].$h[3]." according to TS 04.08 [15]";
    my $loci1 = "Location Area Information LAI: ".$h[4].$h[5].$h[6].$h[7].$h[8]." according to TS 04.08 [15]";
    my $loci2 = "Current value of Periodic Location Updating Timer (T3212): ".$h[9].
    " This byte is used by Phase 1 MEs, but it shall not be used by Phase 2 MEs.";
    my @status = split (//,Hex2Bin($h[10]));
    if ($status[0] eq '0'){
        if (($status[5] eq '0')&&($status[6] eq '0')){$lus = "updated";}
        if (($status[5] eq '0')&&($status[6] eq '1')){$lus = "not updated";}
        if (($status[5] eq '1')&&($status[6] eq '0')){$lus = "PLMN not allowed";}
        if (($status[5] eq '1')&&($status[6] eq '1')){$lus = "Location Area not allowed";}
    }
    if ($status[0] eq '1'){
        $lus = "reserved";   
    }
    my $loci3 = "Location update status: ".$lus." status of location update according to TS 04.08 [15]";    
    $loci=$loci0."###".$loci1."###".$loci2."###".$loci3;
    return $loci; 
}
################################################################################################
sub Parse_AD {
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.67
    #This EF contains information concerning the mode of operation according to the type of SIM.
    
    my @h = @_;
    
    my $ad0 = "Initial value is: ".$h[0];
    my $ad1;
    #mode of operation for the MS
        if ($h[0]==00){
            $ad1 = "Normal operation!";
        }
        if ($h[0]==80){
            $ad1 = "Type approval operations!";
        }
        if ($h[0]==01){
            $ad1 = "Normal operations + specific facilities!";
        }
        if ($h[0]==81){
            $ad1 = "Type approval operations + specific facilities!";
        }
        if ($h[0]==02){
            $ad1 = "Maintenance(off line)!";
        }
        if ($h[0]==04){
            $ad1 = "Cell test operation!";
        }    
        my @temp1=split(//,Hex2Bin($h[0]));
        my @temp2=split(//,Hex2Bin($h[1])); 
        my @temp3=split(//,Hex2Bin($h[2]));
        
        my $ad2 = "Additional information: ";
        #specific facilities (if b1=1 in byte 1);
        my ($ad3, $ad4);
        if ($temp1[0]==0){
            $ad3 = "Specific facilities";
        }
        if ($temp1[1]==1){
            $ad3 = "ME manufacturer specific information";
        }    
        if ($temp3[0]==0){
            $ad4 = "OFM to be disabled by the ME!";
        }
        if ($temp3[0]==1){
            $ad4 = "OFM to be activated by the ME!";
        }
        my $ad5 = "RFU: ";    
        my $i;
        my $ad6 = " ";
        for ($i=0;$i<8;$i++){
            $ad6 = $ad6.$temp2[$i];
        }
        my $ad7 = " ";
        for ($i=1;$i<8;$i++){
            $ad7 = $ad7.$temp3[$i];
        }    
        my $ad8 = "Length of MNC in the IMSI: ";
        
        my @len=split(//,Hex2Bin($h[2]));
        my $ad9 = $len[4].$len[5].$len[6].$len[7];
        
    return $ad = $ad0.$ad1."###".$ad2.$ad3.$ad4."###".$ad5.$ad6.$ad7."###".$ad8.$ad9."###";
}
################################################################################################
sub Parse_Phase {
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.68
    #This EF contains information concerning the phase of the SIM.
    
    my @h = @_;
    
    my $phase0 ="SIM Phase: ".$h[0];
    my $phase1;
    if ($h[0]==00){
       $phase1 = "Phase1";
    }
    elsif ($h[0]==02){
       $phase1 = "Phase2";
    }
    elsif ($h[0]==03){
       $phase1 = "Phase2 and PROFILE DOWNLOAD required";
    }
    else {
       $phase1 = "Reserved for specification by ETSI TC SMG";
    }
    $phase=$phase0."###".$phase1;
    #This phase identification does not preclude a SIM to support some features of a phase later than the one indicated in
    #EFPhase.
    return $phase;     
} 
################################################################################################
sub Parse_VGCS {
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.69
    #This EF contains a list of those VGCS group identifiers the user has subscribed to. The elementary file is used by the
    #ME for group call establishment and group call reception.
    
    my @h = @_;
    my $i = 0;
    $vgcs = "";
    for ($i=0;$i<(scalar($#h)/4);$i++){
        #VGCS Group ID, according to TS 23.003 [10]
        $vgcs = $vgcs."Group ID ".($i+1)." : ".$h[(4*$i)].$h[(4*$i)+1].$h[(4*$i)+2].$h[(4*$i)+3]."###";
    }
    return $vgcs;    
} 
################################################################################################
sub Parse_VGCSS{
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.70
    #This EF contains the status of activation for the VGCS group identifiers. The elementary file is directly related to the
    #EFVGCS. This EF shall always be allocated if EFVGCS is allocated.
    
    my @h = @_;
    my $i = 0;
    $vgcssz = "";
    for ($i=0;$i<6;$i++){
    my @t = split(//,Hex2Bin($h[$i]));
    my $s0 = ($i*8)+1;
    my $s1 = ($i*8)+2;
    my $s2 = ($i*8)+3;
    my $s3 = ($i*8)+4;
    my $s4 = ($i*8)+5;
    my $s5 = ($i*8)+6;
    my $s6 = ($i*8)+7;
    my $s7 = ($i*8)+8;
    #Activation/Deactivation Flags of the appropriate Group IDs
    $vgcss0  = "A/D flags Group ID ".$s0."  ".$t[7];
    $vgcss1  = "A/D flags Group ID ".$s1."  ".$t[6];
    $vgcss2  = "A/D flags Group ID ".$s2."  ".$t[5];
    $vgcss3  = "A/D flags Group ID ".$s3."  ".$t[4];
    $vgcss4  = "A/D flags Group ID ".$s4."  ".$t[3];
    $vgcss5  = "A/D flags Group ID ".$s5."  ".$t[2];
    $vgcss6  = "A/D flags Group ID ".$s6."  ".$t[1];
    $vgcss7  = "A/D flags Group ID ".$s7."  ".$t[0];
    $vgcssz = $vgcssz.$vgcss0."###".$vgcss1."###".$vgcss2."###".$vgcss3."###".
    $vgcss4."###".$vgcss5."###".$vgcss6."###".$vgcss7."###";
    } 
    my @t1 = split(//,Hex2Bin($h[6]));
    $vgcss8 = "A/D flags Group ID "."49"."  ".$t1[7];
    $vgcss9 = "A/D flags Group ID "."50"."  ".$t1[6];
    $vgcss = $vgcssz.$vgcss8."###".$vgcss9."###";
    return $vgcss;
}
################################################################################################
sub Parse_VBS {
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.71
    #This EF contains a list of those VBS group identifiers the user has subscribed to. The elementary file is used by the ME
    #for broadcast call establishment and broadcast call reception.
    
    my @h = @_;
    my $fine=scalar($#h);
    my @group;
    my $i=0;
    for ($i=0;$i<($fine/4);$i++){ #$fine/4 because each group is composed about 4 bytes
        $group[$i]=($h[4*$i].$h[(4*$i)+1].$h[(4*$i)+2].$h[(4*$i)+3]);
        } 
    $vbs = "";#VBS Group ID, according to TS 23.003 [10]
    for ($i=0;$i<($fine/4);$i++){
        $vbs=$vbs."Group ID: ".($i+1).$group[$i]."###";
        }
    return $vbs;         
}
################################################################################################
sub Parse_VBSS{
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.73
    #This EF contains the status of activation for the VBS group identifiers. The elementary file is directly related to the
    #EFVBS. This EF shall always be allocated if EFVBS is allocated.
    
    my @h = @_;
    my $i = 0;
    $vbssz = "";
    for ($i=0;$i<6;$i++){
    my @t = split(//,Hex2Bin($h[$i]));
    my $s0 = ($i*8)+1;
    my $s1 = ($i*8)+2;
    my $s2 = ($i*8)+3;
    my $s3 = ($i*8)+4;
    my $s4 = ($i*8)+5;
    my $s5 = ($i*8)+6;
    my $s6 = ($i*8)+7;
    my $s7 = ($i*8)+8;
    #Activation/Deactivation Flags of the appropriate Group IDs
    $vbss0  = "A/D flags Group ID ".$s0."  ".$t[7];
    $vbss1  = "A/D flags Group ID ".$s1."  ".$t[6];
    $vbss2  = "A/D flags Group ID ".$s2."  ".$t[5];
    $vbss3  = "A/D flags Group ID ".$s3."  ".$t[4];
    $vbss4  = "A/D flags Group ID ".$s4."  ".$t[3];
    $vbss5  = "A/D flags Group ID ".$s5."  ".$t[2];
    $vbss6  = "A/D flags Group ID ".$s6."  ".$t[1];
    $vbss7  = "A/D flags Group ID ".$s7."  ".$t[0];
    $vbssz = $vbssz.$vbss0."###".$vbss1."###".$vbss2."###".$vbss3."###".$vbss4."###".
    $vbss5."###".$vbss6."###".$vbss7."###";
    } 
    my @t1 = split(//,Hex2Bin($h[6]));
    $vbss8 = "A/D flags Group ID "."49"."  ".$t1[7];
    $vbss9 = "A/D flags Group ID "."50"."  ".$t1[6];
    $vbss = $vbssz.$vbss8."###".$vbss9."###";
    return $vbss;
}
################################################################################################
sub Parse_eMLPP{
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.73
    #This EF contains information about priority levels and fast call set-up conditions for the enhanced Multi Level Preemption
    #and Priority service that which can be used by the subscriber.
    
    my @h = @_;
    my @t1 = split(//,Hex2Bin($h[0]));
    my @t2 = split(//,Hex2Bin($h[1]));
    
    #The eMLPP priority levels subscribed to.
    $emlpp0 = "priority level A: ".$t1[7];
    $emlpp1 = "priority level B: ".$t1[6];
    $emlpp2 = "priority level 0: ".$t1[5];
    $emlpp3 = "priority level 1: ".$t1[4];
    $emlpp4 = "priority level 2: ".$t1[3];
    $emlpp5 = "priority level 3: ".$t1[2];
    $emlpp6 = "priority level 4: ".$t1[1];
    
    #For each eMLPP priority level, the capability to use a fast call set-up procedure.
    $emlpp7 = "fast call set-up condition for priority level A: ".$t2[7];
    $emlpp8 = "fast call set-up condition for priority level B: ".$t2[6];
    $emlpp9 = "fast call set-up condition for priority level 0: ".$t2[5];
    $emlpp10 = "fast call set-up condition for priority level 1: ".$t2[4];
    $emlpp11 = "fast call set-up condition for priority level 2: ".$t2[3];
    $emlpp12 = "fast call set-up condition for priority level 3: ".$t2[2];
    $emlpp13 = "fast call set-up condition for priority level 4: ".$t2[1];
    
    $emlpp = $emlpp0."###".$emlpp1."###".$emlpp2."###".$emlpp3."###".
    $emlpp4."###".$emlpp5."###".$emlpp6."###".$emlpp7."###".$emlpp8."###".$emlpp9."###".
    $emlpp10."###".$emlpp11."###".$emlpp12."###".$emlpp13."###";
    return $emlpp;
}
################################################################################################
sub  Parse_AAeM{
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.74
    #This EF contains those priority levels (of the Multi Level Pre-emption and Priority service) for which the mobile station
    #shall answer automatically to incoming calls.
    
    my @h = @_;
    my @t = split(//,Hex2Bin($h[0]));
    #For each eMLPP priority level, the capability for the mobile station to answer automatically to incoming
    #calls (with the corresponding eMLPP priority level).
    $aaem = "Automatic answer priority for priority level A: ".$t[7]."###".
    "Automatic answer priority for priority level B: ".$t[6]."###".
    "Automatic answer priority for priority level 0: ".$t[5]."###".
    "Automatic answer priority for priority level 1: ".$t[4]."###".
    "Automatic answer priority for priority level 2: ".$t[3]."###".
    "Automatic answer priority for priority level 3: ".$t[2]."###".
    "Automatic answer priority for priority level 4: ".$t[1]."###";
    return $aaem;
}
################################################################################################
sub Parse_ECC{
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.75
    #This EF contains up to 5 emergency call codes.
    
    my @h = @_;
    
    my $i=0;
    my $a = 0;
    $ecc = " ";
    #The emergency call code is of a variable length with a maximum length of 6 digits. Each emergency call
    #code is coded on three bytes, with each digit within the code being coded on four bits as shown below. If
    #a code of less that 6 digits is chosen, then the unused nibbles shall be set to 'F'.
    for ($i=0;$i<5;$i++){
        my @t = split(//,$h[$a]);
        my @u = split(//,$h[$a+1]);
        my @v = split(//,$h[$a+2]);
        my $s = $i+1;        
#        $ecc = $ecc."Emergency Call Code".$s." :".(Hex2Dec($t[0])+Hex2Dec($t[1])).
#       (Hex2Dec($u[0])+Hex2Dec($u[1])).(Hex2Dec($v[0])+Hex2Dec($v[1]))."###";
        $a = $a + 3;           
    }
    return $ecc;
}
################################################################################################
sub Parse_CBMIR {
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.77
    #This EF contains ranges of cell broadcast message identifiers that the subscriber wishes the MS to accept.
    
    my @h = @_; 
    my $i=0;
    $cbmir = " ";
    for ($i=0;$i<(scalar($#h)+1)/4;$i++){
    my $s = $i+1;
    $cbmir = $cbmir."CB Message Identidier ranges ".$s.": ".$h[$i*4].$h[($i*4)+1].$h[($i*4)+2].$h[($i*4)+3]."###";
    }
    #Cell Broadcast Message Identifier Ranges
    return $cbmir;
}
################################################################################################
sub Parse_NIA {
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.79
    #This EF contains categories and associated text related to the Network's indication of alerting in the MS service defined
    #in TS 02.07 [3].

    my $ref = shift(@_);
    my @h = @$ref;
    my $counter = shift(@_);
    
    $nia1 = " ";
    #category of alerting for terminating traffic.
    #according to TS 04.08 [15]. Value 'FF' means that no information on alerting category is available.
    $nia0 = "Category of alerting for terminating traffic: ".$h[0];
    if ($h[0] eq 'FF'){
        $nia1 = "No information on alerting category is aviable!";
    }
    #text describing the type of terminating traffic associated with the category.
    #see the coding of the Alpha Identifier item of the EFADN (subclause 10.5.1). The maximum number of
    #characters for this informative text is indicated in TS 02.07 [3].
    $nia2 = "Text describing the type of terminating traffic associated with the category: ";
    my $i;
    $nia3 = " ";
    for ($i=1;$i<scalar($#h);$i++){
        $nia3 = $nia3.$h[$i];
    }
    return $nia[$counter]=$nia0.$nia1."###".$nia2.$nia3;
}
################################################################################################
sub Parse_KcGPRS{
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.79
    #This EF contains the ciphering key KcGPRS and the ciphering key sequence number n for GPRS (see TS 23.060 [32]).
    
    my @h = @_;
    #The least significant bit of KcGPRS is the least significant bit of the eighth byte. The most significant bit
    #of KcGPRS is the most significant bit of the first byte.
    $kcgprs0 = "Ciphering key KcGPRS: ";
    my $i=0;
    $kcgprs1 = " ";
    for ($i=0;$i<8;$i++){
        $kcgprs1 = $kcgprs1.Hex2Bin($h[$i]);
    }
    $kcgprs2 = "Ciphering Key sequence number n for GPRS: ";
    my @t = split(//,Hex2Bin($h[8]));
    #bits b4 to b8 are coded 0
    $kcgprs3 = $t[5].$t[6].$t[7];
    $kcgprs = $kcgprs0.$kcgprs1."###".$kcgprs2.$kcgprs3."###";
    return $kcgprs;
}
################################################################################################
sub Parse_LOCIGPRS{
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.80
    #This EF contains the Location Information.
    
    my @h = @_;
    my @t = split(//,Hex2Bin($h[13]));
    #Packet Temporary Mobile Subscriber Identity according to TS 04.08 [15].
    $locigprs0 = "P-TMSI Packet Temporary Mobile Subscriber Identity: ".
    Hex2Bin($h[0]).Hex2Bin($h[1]).Hex2Bin($h[2]).Hex2Bin($h[3])."###".
    #Packet Temporary Mobile Subscriber Identity signature value according to TS 04.08 [15]
    "P-TMSI signature value: ".
    Hex2Bin($h[4]).Hex2Bin($h[5]).Hex2Bin($h[6])."###".
    #according to TS 04.08 [15].
    "RAI Routing Area Information: ".
    Hex2Bin($h[7]).Hex2Bin($h[8]).Hex2Bin($h[9]).Hex2Bin($h[10]).Hex2Bin($h[11]).Hex2Bin($h[12]).
    "RFU is: ".$t[0].$t[1].$t[2].$t[3].$t[4]."###".
    #status of routing area update according to TS 04.08 [15].
    "Routing Area update status: ";

    if ($t[5] == 0){
        if  ($t[6] == 0){
            if ($t[7] == 0){
                $locigprs1 = "Updated";
            }
            if ($t[7] == 1){
                $locigprs1 = "Not updated";
            }
        }
        if ($t[6] == 1){
            if ($t[7] == 0){
                $locigprs1 = "PLMN not allowed";
            }
            if ($t[7] == 1){
                $locigprs1 = "Routing Area not allowed";
            }
        }   
    }
    if ($t[5] == 1){
        if ($t[6] == 1){
            if ($t[7] == 1){
                $locigprs1 = "Reserved";
            }
        }
    }
    $locigprs = $locigprs0.$locigprs1."###";
    return $locigprs;
}
################################################################################################
sub Parse_SUME {
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.81
    #This EF contains Simple TLVs related to the menu title to be used by a SIM card supporting the SIM API when issuing
    #a SET UP MENU proactive command.
    
    my @h = @_;
    $sume = @h;
    #this field contains the Alpha Identifier Simple TLV defining the menu title text.according to TS 11.14 [27]
    return $sume;
}
################################################################################################
sub Parse_PLMNwACT{
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.82
    #This EF contains coding for n PLMNs, where n is at least eight. This information, determined by the user, defines the
    #preferred PLMNs of the user in priority order. The EF also contains the Access Technologies for each PLMN in this
    #list. (see TS 23.122 [51]).
    
    my @h = @_;
    $plmnwact = " ";
    my $i = 0;
    for ($i=0;$i<((scalar($#h)+1)/5);$i++){
        my @t = split(//,Hex2Bin($h[(5*$i)+4]));
        my $s = $i+1;
        #PLMN: Mobile Country Code (MCC) followed by the Mobile Network Code (MNC).according to TS 24.008 [47]
        #Access Technology The Access Technology of the HPLMN that the MS will assume when searching for the HPLMN, in
        #priority order. The first Access Technology in the list has the highest priority
        my @plmn1 = split(//,$h[5*$i]);
        my @plmn2 = split(//,$h[(5*$i)+1]);
        my @plmn3 = split(//,$h[(5*$i)+2]);
        $plmnwact = $plmnwact.$s." PLMN: ".$mnc{$plmn1[1].$plmn1[0].$plmn2[1].$plmn2[0].$plmn3[1].$plmn3[0]}."###".
        "Access Technologies of ".$s." PLMN in PLMN selector with Access Technology."."###".
        "RFU is: ".$t[0].$t[1].$t[2].$t[3].$t[4].$t[5]."###".
        "CPBCCH (COMPACT network): ".$act{$t[6]}."###".
        "BCCH (GSM network): ".$act{$t[7]}."###";    
    }
    return $plmnwact;
}   
################################################################################################
sub Parse_OPLMNwACT{
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.83
    #This EF contains coding for n PLMNs, where n is at least eight. This information, determined by the operator, defines
    #the preferred PLMNs of the operator in priority order. The EF also contains the Access Technologies for each PLMN in
    #this list (see TS 23.122 [51]).
    
    my @h = @_;
    $oplmnwact = " ";
    my $i = 0;
    for ($i=0;$i<((scalar($#h)+1)/5);$i++){
        my $s = $i+1;
        my @t = split(//,Hex2Bin($h[(5*$i)+4]));
        #PLMN: Mobile Country Code (MCC) followed by the Mobile Network Code (MNC).according to TS 24.008 [47]
        #Access Technology The Access Technology of the HPLMN that the MS will assume when searching for the HPLMN, in
        #priority order. The first Access Technology in the list has the highest priority
        my @plmn1 = split(//,$h[5*$i]);
        my @plmn2 = split(//,$h[(5*$i)+1]);
        my @plmn3 = split(//,$h[(5*$i)+2]);
        $oplmnwact = $oplmnwact.$s." PLMN: ".$mnc{$plmn1[1].$plmn1[0].$plmn2[1].$plmn2[0].$plmn3[1].$plmn3[0]}."###".
        "Access Technologies of ".$s." PLMN in Operator controlled PLMN selector with Access Technology.".
        "RFU is: ".$t[0].$t[1].$t[2].$t[3].$t[4].$t[5]."###".
        "CPBCCH (COMPACT network): ".$act{$t[6]}."###".
        "BCCH (GSM network): ".$act{$t[7]}."###";    
    }
    #Operator Controlled PLMN (Selector List)
    return $oplmnwact;
}   
################################################################################################
sub Parse_HPLMNwACT {
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.84
    #The HPLMN Selector with access technology data field shall contain the HPLMN code, or codes together with the
    #respective access technology in priority order (see TS 23.122 [51]).
    
    my @h = @_;
    $hplmnwact = " ";
    my $i = 0;
    for ($i=0;$i<((scalar($#h)+1)/5);$i++){
        my @t = split(//,Hex2Bin($h[(5*$i)+4]));
        my $s = $i+1;
        #PLMN: Mobile Country Code (MCC) followed by the Mobile Network Code (MNC).according to TS 24.008 [47]
        #Access Technology The Access Technology of the HPLMN that the MS will assume when searching for the HPLMN, in
        #priority order. The first Access Technology in the list has the highest priority
        my @plmn1 = split(//,$h[5*$i]);
        my @plmn2 = split(//,$h[(5*$i)+1]);
        my @plmn3 = split(//,$h[(5*$i)+2]);
        $hplmnwact = $hplmnwact.$s." PLMN: ".$mnc{$plmn1[1].$plmn1[0].$plmn2[1].$plmn2[0].$plmn3[1].$plmn3[0]}."###".
        "Access Technologies of ".$s." PLMN in Operator controlled PLMN selector with Access Technology.".
        "RFU is: ".$t[0].$t[1].$t[2].$t[3].$t[4].$t[5]."###".
        "CPBCCH (COMPACT network): ".$act{$t[6]}."###".
        "BCCH (GSM network): ".$act{$t[7]}."###";
    }
    #Home PLMN
    return $hplmnwact;
}
################################################################################################
sub Parse_CPBCCH{
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.84
    #This EF contains information concerning the CPBCCH according to TS 04.18 [48] and TS 03.22 [45].
    
    my @h = @_;
    $cpbcch = " ";
    my $i=0;
    my $a=0;
    for ($i=1;$i<=(scalar($#h)+1)/2;$i++){        
        my $cpbcch0 = "Element ".$i." of CPBCCH carrier list: ";
        my @t = split(//,($h[$a]));
        my @s = split(//,Hex2Bin($t[0]));
        my @u = split(//,Hex2Bin($t[1]));
        my @t1 = split(//,($h[$a+1]));
        my @s1 = split(//,Hex2Bin($t1[0]));
        my @u1 = split(//,Hex2Bin($t1[1]));
        #ARFCN (10 bits) as defined in TS 05.05 [46].
        my $cpbcch1 = "ARFCN: ".$u[3].$u[2].$u[1].$u[0].$s[3].$s[2].$s[1].$s[0].$u1[3].$u1[2];
        $a = $a + 2;
        my $cpbcch2 = "High/Low band indicator: ".$act{$u1[1]};
        my $cpbcch3 = "RFU: ".$u1[0].$s1[3].$s1[2].$s1[1];
        my $cpbcch4 = "Empty indicator: ".$act{$s1[0]};
        $cpbcch = $cpbcch.$cpbcch0."###".$cpbcch1."###".$cpbcch2."###".$cpbcch3."###".$cpbcch4."###";
    }
    return $cpbcch;
}
################################################################################################
sub Parse_INVSCAN{
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.85
    #This EF contains two flags used to control the investigation scan for higher prioritized PLMNs not offering voice
    #services.
    
    my @h = @_;
    my @t = split(//,(Hex2Bin($h[0])));
    #A '1' in a bit position indicates that the investigation scan shall be performed for the condition corresponding to that bit
    #position and a '0' that it shall not be performed.
    $invscan = "In limited service mode: ".$t[7]."###"."After successful PLMN selection: ".$t[6]."###".
    "RFU is: ".$t[5].$t[4].$t[3].$t[2].$t[1].$t[0]."###";
    return $invscan;
}

################################################################################################
sub Parse_SAI{
    
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.86
    #This EF contains the 'LSA only access indicator'. This EF shall always be allocated if DFSoLSA is present.
    
    my @h = @_;
    
    my @t = split(//,(Hex2Bin($h[0])));
    my $sai0 = " ";
    #indicates whether the MS is restricted to use LSA cells only or not.
    if ($t[7] eq '1'){
        $sai0="LSA only access activated";
    }
    if ($t[7] eq '0'){
        $sai0="LSA only access not activated";
    }
    my $i=0;
    my @text_bin_SAI;
    my @bit_text;
    my $sai1 = " ";
    for ($i=1;$i<=scalar($#h);$i++){
        $text_bin_SAI[$i]=Hex2Bin($h[$i]);
        @bit_text=(split(//,$text_bin_SAI[$i]));
        #text to be displayed by the ME when it's out of LSA area.
        $sai1 = $sai1.Trad2Sms(join('',$bit_text[1],$bit_text[2],$bit_text[3],$bit_text[4],$bit_text[5],$bit_text[6],$bit_text[7]));
    }
    $sai = $sai0."###".$sai1;
    return $sai;
    }
################################################################################################
sub Parse_SLL{
    
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.87
    #This EF contains information describing the LSAs that the user is subscribed to. This EF shall always be allocated if
    #DFSoLSA is present.
    
    my $ref = shift(@_);
    my @h = @$ref;
    my $counter = shift(@_);
    
    my $i=0;
    my $text_bin_SLL;
    my @bit_text;
    my $sll1 = " ";
    #LSA name string to be displayed when the ME is camped in the corresponding area, dependant on the
    #contents of the LSA indication for idle mode field.
    for ($i=0;$i<=(scalar($#h)-10);$i++){
        $text_bin_SLL[$i]=Hex2Bin($h[$i]);
        @bit_text=(split(//,$text_bin_SLL[$i]));
        $sll1 = $sll1.Trad2Sms(join('',$bit_text[1],$bit_text[2],$bit_text[3],$bit_text[4],$bit_text[5],$bit_text[6],$bit_text[7]));
    }
    #Icon qualifier, control of idle mode support and control of LSA indication for idle mode.
    my $sll2a = "Configuration parameters: ";
    my $sll2b;
    #The icon qualifier indicates to the ME how the icon is to be used.
    my @bit_conf = split(//,(Hex2Bin($h[scalar($#h)-9])));
    if (($bit_conf[6] eq '0')&&($bit_conf[7] eq '0')){
        $sll2b = "icon is not to be used and may not be present";
    }
    if (($bit_conf[6] eq '0')&&($bit_conf[7] eq '1')){
        $sll2b = "icon is self-explanatory, i.e. if displayed, it replaces the LSA name";
    }
    if (($bit_conf[6] eq '1')&&($bit_conf[7] eq '0')){
        $sll2b = "icon is not self-explanatory, i.e. if displayed, it shall be displayed together with the LSA name";
    }
    if (($bit_conf[6] eq '1')&&($bit_conf[7] eq '1')){
        $sll2b = "RFU";
    }
    #The idle mode support is used to indicate whether the ME shall favour camping on the LSA cells in
    #idle mode.
    my $sll2c = " ";
    if ($bit_conf[5] eq '0'){
        $sll2c = "Idle mode support disabled";
    }
    if ($bit_conf[5] eq '1'){
        $sll2c = "Idle mode support enabled";
    }
    #The LSA indication for idle mode is used to indicate whether or not the ME shall display the LSA
    #name when the ME is camped on a cell within the LSA.
    my $sll2d = " ";
    if ($bit_conf[4] eq '0'){
        $sll2d = "LSA indication for idle mode disabled";
    }
    if ($bit_conf[4] eq '1'){
        $sll2d = "LSA indication for idle mode enabled";
    }
    
    my $sll2e = "RFU: ".$bit_conf[3].$bit_conf[2].$bit_conf[1].$bit_conf[0];
    
    my $sll2 = $sll2a.$sll2b."###".$sll2c."###".$sll2d."###".$sll2e;
    
    my $sll3 = "RFU: ".$h[scalar($#h)-8];
    
    #The icon identifier addresses a record in EFIMG.
    my $sll4 = "The icon identifier addresses a record in EFIMG: ".$h[scalar($#h)-7];
    
    
    my @bin_prior = split(//,(Hex2Bin($h[scalar($#h)-6])));
    #Priority of the LSA which gives the ME the preference of this LSA relative to the other LSAs
    my $sll5a = "Priority: ".$bin_prior[4].$bin_prior[5].$bin_prior[6].$bin_prior[7]."###";
    my $sll5b = "RFU: ".$bin_prior[0].$bin_prior[1].$bin_prior[2].$bin_prior[3]."###";
    my $sll5 = "Priority of the LSA which gives the ME the preference of this LSA relative to the other LSAs: ".$sll5a.$sll5b;
    
    my $sll6 = "PLMN code(MCC + MNC for the LSA): ".$h[scalar($#h)-5].$h[scalar($#h)-4].$h[scalar($#h)-3];
    #these bytes identify the EF which contains the LSA Descriptors forming the LSA.
    my $sll7 = "These bytes identify the EF which contains the LSA Descriptors forming the LSA: "." high byte of the LSA Descriptor file ".$h[scalar($#h)-2]." low byte of the LSA Descriptor file ".$h[scalar($#h)-1];
    #this byte identifies the number of the first record in the LSA Descriptor file forming the LSA.
    my $sll8 = "This byte identifies the number of the first record in the LSA Descriptor file forming the LSA: ".$h[scalar($#h)];

    return $sll[$counter] = $sll1.$sll2."###".$sll3."###".$sll4."###".$sll5."###".$sll6."###".$sll7."###".$sll8;
    }
################################################################################################
sub Parse_MExEST{
    
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.90
    #This EF indicates which MExE services are allocated, and whether, if allocated, the service is activated. If a service is
    #not allocated or not activated in the SIM, the ME shall not select this service.
    
    my @h = @_;
    my %results = ('00' =>"service not allocated",
                   '01' =>"service not allocated",
                   '10' =>"service allocated but not activated",
                   '11' =>"service allocated and activated",);
    
    my @out1 = split(//,Hex2Bin($h[0]));
    
    return $mexest = "Service n1 : Operator root public key ".$results{$out1[7].$out1[6]}."###".
    "Service n2 : Administrator root public key ".$results{$out1[5].$out1[4]}."###".
    "Service n3 : Third party root public key ".$results{$out1[3].$out1[2]}."###".
    "Service n4 : RFU ".$results{$out1[1].$out1[0]};

    }
################################################################################################
sub Parse_ORPK{
    
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.91
    #This EF contains the descriptor(s ) of certificates containing the Operator Root Public Key. This EF shall only be
    #allocated if the operator wishes to verify applications and certificates in the MExE operator domain using a root public
    #key held on the SIM. Each record of this EF contains one certificate descriptor.
    
    my $ref = shift(@_);
    my @h = @$ref;
    my $counter = shift(@_);
    
    #$h[0] for parameters indicator
    my @pm = split(//,Hex2Bin($h[0]));
    #The parameter indicator indicates if record is full and which optional parameters are present
    my $orpk1 = "Parameters indicator: "."Certificate descriptor is valid (bit1=0 key
                descriptor is valid)".$pm[7]."Reserved bit set to 1 (bitx=0 optional parameter
                present)".$pm[0].$pm[1].$pm[2].$pm[3].$pm[4].$pm[5].$pm[6];
    
    #$h[1] for flags
    my @flags = split(//,Hex2Bin($h[1]));
    #The authority flag indicates whether the certificate identify an authority (i.e. CA or AA) or not.
    my $orpk2 = "Authority certificate (bit=1 certificate of an authority): ".$flags[7];
    
    #$h[2] for Type of certificate
    my $toc_ris=" ";
    my @toc = split(//,Hex2Bin($h[2]));
    #This field indicates the type of certificate containing the key.
    if (($toc[7] eq '0') && ($toc[6] eq '0')){$toc_ris="WTLS";}
    if (($toc[7] eq '0') && ($toc[6] eq '1')){$toc_ris="X509";}
    if (($toc[7] eq '1') && ($toc[6] eq '0')){$toc_ris="X9.68";}
    my $orpk3 = "Type of certificate: ".$toc_ris;
    
    #$h[3] and $h[4] for Key/certificate file identifier
    #these bytes identify an EF which is the key/certificate data file (see subclause 10.7.5), holding the actual
    #key/certificate data for this record.
    my $orpk4 = "high byte of Key/certificate File Identifier: ".$h[3]." low byte of Key/certificate File Identifier: ".$h[4];
    
    #$h[5] and $h[6] for Offset into Key/certificate File
    #these bytes specify an offset into the transparent key/certificate data File identified in bytes 4 and 5.
    my $orpk5 = "high byte of offset into Key/certificate Data File: ".$h[5]." low byte of offset into Key/certificate Data File: ".$h[6];
    
    #$h[7] and $h[8] for Length of Key/certificate Data
    #these bytes yield the length of the key/certificate data, starting at the offset identified in "Offset into
    #Key/certificate File" field
    my $orpk6 = "high byte of Key/certificate Data length: ".$h[7]." low byte of Key/certificate Data length: ".$h[8];
    
    #$h[9] for Key identifier length
    #This field gives length of key identifier
    my $orpk7 = "Key identifier length: ".Hex2Bin($h[9]);
    
    #Key identifier
    my $orpk8 = "Key identifier: ";
    #This field provides a means of identifying certificates that contain a particular public key (chain building) and
    #linking the public key to its corresponding private key. For more information about value and using see
    #TS 23.057 [50]
    my $i = 10;
    my $orpk9="";
    for ($i=10;$i<=scalar($#h);$i++){
        $orpk9=$orpk9.$h[$i];
        }
    return $orpk[$counter]=$orpk1."###".$orpk2."###".$orpk3."###".$orpk4."###".$orpk5."###".$orpk6."###".$orpk7."###".$orpk8.$orpk9;
    }
################################################################################################
sub Parse_ARPK{
    
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.93
    #This EF contains the descriptor(s ) of certificates containing the Administrator Root Public Key. This EF shall only be
    #allocated if the SIM issuer wishes to control the Third Party certificates on the terminal using an Administrator Root
    #Public Key held on the SIM. Each record of this EF contains one certificate descriptor.
    
    my $ref = shift(@_);
    my @h = @$ref;
    my $counter = shift(@_);
    
    #$h[0] for parameters indicator
    my @pm = split(//,Hex2Bin($h[0]));
    #The parameter indicator indicates if record is full and which optional parameters are present
    my $arpk1 = "Parameters indicator: "."Certificate descriptor is valid (bit1=0 key
                descriptor is valid)".$pm[7]."Reserved bit set to 1 (bitx=0 optional parameter
                present)".$pm[0].$pm[1].$pm[2].$pm[3].$pm[4].$pm[5].$pm[6];
    
    #$h[1] for flags
    my @flags = split(//,Hex2Bin($h[1]));
    #The authority flag indicates whether the certificate identify an authority (i.e. CA or AA) or not.
    my $arpk2 = "Authority certificate (bit=1 certificate of an authority): ".$flags[7];
    
    #$h[2] for Type of certificate
    my $toc_ris=" ";
    my @toc = split(//,Hex2Bin($h[2]));
    #This field indicates the type of certificate containing the key.
    if (($toc[7] eq '0') && ($toc[6] eq '0')){$toc_ris="WTLS";}
    if (($toc[7] eq '0') && ($toc[6] eq '1')){$toc_ris="X509";}
    if (($toc[7] eq '1') && ($toc[6] eq '0')){$toc_ris="X9.68";}
    my $arpk3 = "Type of certificate: ".$toc_ris;
    
    #$h[3] and $h[4] for Key/certificate file identifier
    #these bytes identify an EF which is the key/certificate data file (see subclause 10.7.5), holding the actual
    #key/certificate data for this record.
    my $arpk4 = "high byte of Key/certificate File Identifier: ".$h[3]." low byte of Key/certificate File Identifier: ".$h[4];
    
    #$h[5] and $h[6] for Offset into Key/certificate File
    #these bytes specify an offset into the transparent key/certificate data File identified in bytes 4 and 5.
    my $arpk5 = "high byte of offset into Key/certificate Data File: ".$h[5]." low byte of offset into Key/certificate Data File: ".$h[6];
    
    #$h[7] and $h[8] for Length of Key/certificate Data
    #these bytes yield the length of the key/certificate data, starting at the offset identified in "Offset into
    #Key/certificate File" field
    my $arpk6 = "high byte of Key/certificate Data length: ".$h[7]." low byte of Key/certificate Data length: ".$h[8];
    
    #$h[9] for Key identifier length
    #This field gives length of key identifier
    my $arpk7 = "Key identifier length: ".Hex2Bin($h[9]);
    
    #Key identifier
    my $arpk8="Key identifier: ";
    #This field provides a means of identifying certificates that contain a particular public key (chain building) and
    #linking the public key to its corresponding private key. For more information about value and using see
    #TS 23.057 [50]
    my $i = 10;
    my $arpk9="";
    for ($i=10;$i<=scalar($#h);$i++){
        $arpk9=$arpk9.$h[$i];
        }
    return $arpk[$counter]=$arpk1."###".$arpk2."###".$arpk3."###".$arpk4."###".$arpk5."###".$arpk6."###".$arpk7."###".$arpk8.$arpk9;
    }
################################################################################################
sub Parse_TPRPK{
    
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.93
    #This EF contains descriptor(s ) of certificates containing the Third Party Root Public key (s). This EF shall only be
    #allocated if the SIM issuer wishes to verify applications and certificates in the MExE Third Party domain using root
    #public key(s) held on the SIM. This EF can contain one or more root public keys. Each record of this EF contains one
    #certificate descriptor.
    
    my $ref = shift(@_);
    my @h = @$ref;
    my $counter = shift(@_);
    
    #$h[0] for parameters indicator
    my @pm = split(//,Hex2Bin($h[0]));
    #The parameter indicator indicates if record is full and which optional parameters are present
    my $trpk1 = "Parameters indicator: "."Certificate descriptor is valid (bit1=0 key
                descriptor is valid)".$pm[7]."Reserved bit set to 1 (bitx=0 optional parameter
                present)".$pm[0].$pm[1].$pm[2].$pm[3].$pm[4].$pm[5].$pm[6];
    
    #$h[1] for flags
    my @flags = split(//,Hex2Bin($h[1]));
    #The authority flag indicates whether the certificate identify an authority (i.e. CA or AA) or not.
    my $trpk2 = "Authority certificate (bit=1 certificate of an authority): ".$flags[7];
    
    #$h[2] for Type of certificate
    my $toc_ris=" ";
    my @toc = split(//,Hex2Bin($h[2]));
    #This field indicates the type of certificate containing the key.
    if (($toc[7] eq '0') && ($toc[6] eq '0')){$toc_ris="WTLS";}
    if (($toc[7] eq '0') && ($toc[6] eq '1')){$toc_ris="X509";}
    if (($toc[7] eq '1') && ($toc[6] eq '0')){$toc_ris="X9.68";}
    my $trpk3 = "Type of certificate: ".$toc_ris;
    
    #$h[3] and $h[4] for Key/certificate file identifier
    #these bytes identify an EF which is the key/certificate data file (see subclause 10.7.5), holding the actual
    #key/certificate data for this record.
    my $trpk4 = "high byte of Key/certificate File Identifier: ".$h[3]."low byte of Key/certificate File Identifier: ".$h[4];
    
    #$h[5] and $h[6] for Offset into Key/certificate File
    #these bytes specify an offset into the transparent key/certificate data File identified in bytes 4 and 5.
    my $trpk5 = "high byte of offset into Key/certificate Data File: ".$h[5]."low byte of offset into Key/certificate Data File: ".$h[6];
    
    #$h[7] and $h[8] for Length of Key/certificate Data
    #these bytes yield the length of the key/certificate data, starting at the offset identified in "Offset into
    #Key/certificate File" field
    my $trpk6 = "high byte of Key/certificate Data length: ".$h[7]."low byte of Key/certificate Data length: ".$h[8];
    
    #$h[9] for Key identifier length
    #This field gives length of key identifier
    my $trpk7 = "Key identifier length: ".Hex2Bin($h[9]);
    
    #Key identifier
    #This field provides a means of identifying certificates that contain a particular public key (chain building) and
    #linking the public key to its corresponding private key. For more information about value and using see
    #TS 23.057 [50]
    my $i = 10;
    my $trpk8="";
    my $length_x= 10 + Hex2Dec($h[9]);
    for ($i=10;$i<=$length_x;$i++){
        $trpk8=$trpk8.$h[$i];
        }
    
    #This field gives length of certificate identifier
    my $trpk9= "Certificate identifier length: ".Hex2Bin($h[$length_x+1]);
    
    #This field identify the issuer and provide a easy way to find a certificate. For more information about value
    #and usage, see TS 23.057 [50].
    my $k=0;
    my $trpk10="";
    for ($k=($length_x+2);$k<scalar($#h);$k++){
        $tprk10=$tprk10.$h[$k];
        }
    
    return $trpk[$counter]=$trpk1."###".$trpk2."###".$trpk3."###".$trpk4."###".$trpk5."###".$trpk6."###".$trpk7."###"."Key identifier: ".$trpk8."###".$trpk9."###"."Certificate identifier: ".$trpk10;
    }
################################################################################################
sub Parse_ADN{
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.94
    #This EF contains Abbreviated Dialling Numbers (ADN) and/or Supplementary Service Control strings (SSC). In
    #addition it contains identifiers of associated network/bearer capabilities and identifiers of extension records. It may also
    #contain an associated alpha-tagging.
    
    my $ref = shift(@_);
    my @h = @$ref;
    my $counter = shift(@_);    
    
    my $adn1 = " ";
    my $adn0 = " ";
    
    if ($h[0] eq 'FF'){
        $adn0 = "";
        $adn1 = "Empty";
    }
    else{
    my @chi;
    my $i=scalar($#h)-13;
    my $a=0;
    
    for($a=0;$a<$i;$a++){
            $chi[$a]=$h[$a];
    }
    
    my @chibin;
    my @fin;
    for ($ii=0;$ii<$i;$ii++){
        $chibin[$ii]=Hex2Bin($chi[$ii]);
        @fin=(split(//,$chibin[$ii]));
        #Alpha-tagging of the associated dialling number.
        $adn0 = $adn0.Trad2Sms(join('',$fin[1],$fin[2],$fin[3],$fin[4],$fin[5],$fin[6],$fin[7]));
    }    
    my $c=0;
    for ($c=11;$c>1;$c--){  
        $adnp1 = chop($h[scalar($#h)-$c]);
        $adnp2 = ($h[scalar($#h)-$c]);    
        if ($adnp1 eq 'F'){
            $adnp1 = "";
        }
        if ($adnp2 eq 'F'){
            $adnp2 = "";
        }
        $adn1 =$adn1.$adnp1.$adnp2;
    }
    }
    return $adn[$counter] = $adn0.$adn1;
    }

################################################################################################
sub Parse_FDN{
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.98
    #This EF contains Fixed Dialling Numbers (FDN) and/or Supplementary Service Control strings (SSC). In addition it
    #contains identifiers of associated network/bearer capabilities and identifiers of extension records. It may also contain an
    #associated alpha-tagging.
    
    my $ref = shift(@_);
    my @h = @$ref;
    my $counter = shift(@_);
    
    my $fdn0 = " ";
    my $fdn1 = " ";
    
    if ($h[0] eq 'FF'){
        $fdn0="";
        $fdn1="Empty";
    }
    else{
    my @chi;
    my $i=scalar($#h)-13;
    my $a=0;
    for($a=0;$a<$i;$a++){
        $chi[$a]=$h[$a];
    }
    
    my @chibin;
    my @fin;
    for ($ii=0;$ii<$i;$ii++){
        $chibin[$ii]=Hex2Bin($chi[$ii]);
        @fin=(split(//,$chibin[$ii]));
        #Alpha-tagging of the associated dialling number.
        $fdn0 = $fdn0.Trad2Sms(join('',$fin[1],$fin[2],$fin[3],$fin[4],$fin[5],$fin[6],$fin[7]));
    }    
    
    my $c=0;
    for ($c=11;$c>1;$c--){
        $fdnp1 = chop($h[scalar($#h)-$c]);
        $fdnp2 = ($h[scalar($#h)-$c]);    
        if ($fdnp1 eq 'F'){
            $fdnp1 = "";
        }
        if ($fdnp2 eq 'F'){
            $fdnp2 = "";
        }
        $fdn1 =$fdn1.$fdnp1.$fdnp2;
    }
    }
    return $fdn[$counter] = $fdn0.$fdn1; 
}
################################################################################################

sub Parse_SMS{
    
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.98
    #This EF contains information in accordance with TS 23.040 [13] comprising short messages (and associated
    #parameters) which have either been received by the MS from the network, or are to be used as an MS originated
    #message.
    
    my $ref = shift(@_);
    my @h = @$ref;
    my $counter = shift(@_);
    
    my $message = join('',@h);
    
#    $status = $h[0];
#    my $a=$h[1];
#    #my $b=$h[2];
#    my $c;
#    $num = "";
#    for ($c=1;$c<$a;$c++){
#        $num = $num.chop($h[2+$c]).$h[2+$c];
#    }
#    my $service_number = $num;
#    my $d = $h[3+$c];
#    my @e = split(//,($d));
#    $f = (Hex2Dec($e[1])+Hex2Dec($e[0]*16));
#    my $g=4+$c;
#    my $i;
#    $numt = "";
#    for ($i=1;$i<($f/2)+1;$i++){
#        $numt = $numt.chop($h[$g+$i]).$h[$g+$i];
#    }   
#    my $phone_number = $numt;
#    my $l=$g+$i;
#    my $m=0;
#    for ($m=1;$m<8;$m++){
#        $dat[$m] = chop($h[$l+1+$m]).$h[$l+1+$m];
#    }
#    $datesms = $dat[3]." ".$months{$dat[2]}." ".$dat[1];
#    $hoursms = $dat[4].".".$dat[5].".".$dat[6];
#    my $n = $h[$l+1+$m];
#    my @o = split(//,($n));
#    my $dd = Hex2Dec($o[0]);
#    
#    my $message_length = ((Hex2Dec($o[1]))+($dd*16));
#    #print "Message_length: $message_length\n";
#    
#    my $t1 = $l + 2 + $m;
#    my $t2 = $t1 + $message_length;
#    my $data = " ";    
#    for ($i=$t1; $i<$t2; $i++){
#        $data = $data.$h[$i];
#    }
#        
#    #print "testo: $data\n";
#    
#    my $length  = length($data); 
#    my $message = "";
#    my $len     = length($data);	
#    my $bytes="";
#    my $repeat = int(length($data)) / 2;
#	
#    for($i=0; $i < $repeat; $i++){
#	my $hex = substr($data, $i * 2, 2);
#	my $hex_b = unpack('b8',pack('H2', $hex));
#	$bytes .= $hex_b;
#    }
#    #print "length bytes: ".length($bytes)."\n";
#	
#    #$repeat = $length || int(length($bytes) / 7);
#    $repeat = int(length($bytes) / 7);
#    #$repeat = $length;
#    #print "repeat: $repeat\n";
#    my $last_loop = int(length($bytes) / 7) - 1;
#    
#    for($i = 0; $i < $repeat; $i++){
#    #    print "DEBUG bytes: $bytes\n";
#        my $pos=$i*7;
#        my $letter = substr($bytes, $pos, 7);
#    #    print "DEBUG letter: $letter\n";
#	#if(($i == $last_loop) && ($letter eq '0000000') && (not defined $length)){
#	#	print "Possible edge case, can't be sure if last character is " .
#	#	      'really @ or just a filler.';
#	#}
#	
#        my $tmp = pack('b7', $letter);
#    #    print "DEBUG tmp: $tmp\n";
#	$message = $message.$tmp;
#    }
#    $message=~ s/[^[:print:]]//g;;
#    print "message => $message\n";
#    #$message =~ s/[:^print:]+//g;
#    #print "message => $message\n";   
 
    #return $sms[$counter]={
    #            'Status'                => [$status],             
    #            'Number_Service_Center' => [$service_number],         
    #            'Number'                => [$phone_number],        
    #            'Date'                  => [$datesms],      
    #            'Hour'                  => [$hoursms],    
    #            'Length_SMS'            => [$message_length],
    #            'Text'                  => [$message]          
    #        };
    
    #return $sms[$counter]={
    #            'Text'                  => [$message]          
    #        };
    
    #il messaggio è tradotto nello script di reportistica
    #messaggio incluso nei tag <content>messaggio</content>
    return $sms[$counter]=$message;
}

################################################################################################
sub Parse_CCP{
    
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.99
    #This EF contains parameters of required network and bearer capabilities and ME configurations associated with a call
    #established using an abbreviated dialling number, a fixed dialling number, an MSISDN, a last number dialled, a service
    #dialling number or a barred dialling number.
    
    my $ref = shift(@_);
    my @h = @$ref;
    my $counter = shift(@_);
     
    #see TS 04.08 [15]. The Information Element Identity (IEI) shall be excluded. i.e. the first byte of the
    #EFCCP record shall be Length of the bearer capability contents.
    #Bytes 11-14 shall be set to 'FF' and shall not be interpreted by the ME.
    return $ccp[$counter] = "Bearer capability information element: ".$h[0].$h[1].$h[2].$h[3].$h[4].$h[5].$h[6].$h[7].$h[8].$h[9]."###".
    "Bytes reserved - see below: ".$h[10].$h[11].$h[12].$h[13];
}
################################################################################################
sub Parse_MSISDN{
    
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.100
    #This EF contains MSISDN(s) related to the subscriber. In addition it contains identifiers of associated network/bearer
    #capabilities and identifiers of extension records. It may also contain an associated alpha-tagging.

    my $ref = shift(@_);
    my @h = @$ref;
    my $counter = shift(@_);
    
    my @chi;
    my $i=scalar($#h)-13;
    my $a=0;
    for($a=0;$a<$i;$a++){
        $chi[$a]=$h[$a];
    }
    my $msisdn0 = " ";
    my @chibin;
    my @fin;
    for (my $ii=0;$ii<$i;$ii++){
        $chibin[$ii]=Hex2Bin($chi[$ii]);
        @fin=(split(//,$chibin[$ii]));
        #Alpha-tagging of the associated dialling number.
        $msisdn0 = $msisdn0.Trad2Sms(join('',$fin[1],$fin[2],$fin[3],$fin[4],$fin[5],$fin[6],$fin[7]));
    }    
    my $msisdn1 = " ";
    my $c=0;
    for ($c=11;$c>1;$c--){
        my $msisdnp1 = chop($h[scalar($#h)-$c]);
        my $msisdnp2 = ($h[scalar($#h)-$c]);    
        if ($msisdnp1 eq 'F'){
            $msisdnp1 = " ";
        }
        if ($msisdnp2 eq 'F'){
            $msisdnp2 = " ";
        }
        $msisdn1 =$msisdn1.$msisdnp1.$msisdnp2;
    }       
    return $msisdn[$counter] = $msisdn0.$msisdn1;    
}
################################################################################################
sub Parse_SMSP{
    
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.101
    #This EF contains values for Short Message Service header Parameters (SMSP), which can be used by the ME for user
    #assistance in preparation of mobile originated short messages. For example, a service centre address will often be
    #common to many short messages sent by the subscriber.
    
    my $ref = shift(@_);
    my @h = @$ref;
    my $counter = shift(@_);
    
    my $i=0;
    #Alpha Tag of the associated SMS-parameter.
    $smsp0 = "Alpha-Identifier: ";
    my $cc = scalar($#h)-27;
    for ($i=0;$i<$cc;$i++){
        $smsp0=$smsp0.$h[$i];
    };
    @pam = split(//,Hex2Bin($h[scalar($#h)-27]));
    my $x=0;
    for($x=0;$x<8;$x++){
        if ($pam[$x] eq '0'){$pam[$x]="Parameter present!"};
        if ($pam[$x] eq '1'){$pam[$x]="Parameter absent!"}; 
    };
    #Each of the default SMS parameters which can be stored in the remainder of the record are marked absent
    #or present by individual bits within this byte.
    $smsp1 = "Parameters Indicators: ".$h[scalar($#h)-27]."###".
             #As defined for SM-TL address fields in TS 23.040 [13].
             "TP-Destination Address: ".$pam[7]."###".
             #As defined for RP-Destination address Centre Address in TS 24.011 [16].
             "TS-Service Centre Address: ".$pam[6]."###".
             #As defined in TS 23.040 [13].
             "TP-Protocol Identifier: ".$pam[5]."###".
             #As defined in TS 23.038 [12].
             "TP-Data Coding Scheme: ".$pam[4]."###".
             #As defined in TS 23.040 [13] for the relative time format.
             "TP-Validity Period: ".$pam[3]."###";
       
    $smsp2 = " ";
    $smsp3 = " ";
    $smsp4 = " ";
    $smsp5 = " ";

    if ($pam[7] eq 'Parameter present!'){
        $smsp2 = "TP-Destination Address: ".$h[scalar($#h)-26].$h[scalar($#h)-25].$h[scalar($#h)-24].$h[scalar($#h)-23].$h[scalar($#h)-22].$h[scalar($#h)-21].$h[scalar($#h)-20].$h[scalar($#h)-19].$h[scalar($#h)-18].$h[scalar($#h)-17].$h[scalar($#h)-16].$h[scalar($#h)-15]."###";
    }
    if ($pam[6] eq 'Parameter present!'){
        $smsp3 = "TS-Service Centre Address: ".$h[scalar($#h)-14].$h[scalar($#h)-13].$h[scalar($#h)-12].$h[scalar($#h)-11].$h[scalar($#h)-10].$h[scalar($#h)-9].$h[scalar($#h)-8].$h[scalar($#h)-7].$h[scalar($#h)-6].$h[scalar($#h)-5].$h[scalar($#h)-4].$h[scalar($#h)-3]."###";
    }
    if ($pam[5] eq 'Parameter present!'){
        $smsp4 = "TP-Protocol Identifier: ".$h[scalar($#h)-2]."###";
    }
    if ($pam[4] eq 'Parameter present!'){
        $smsp5 = "TP-Data Coding Scheme: ".$h[scalar($#h)-1]."###";
    }
    if ($pam[3] eq 'Parameter present!'){
        $smsp6 = "TP-Validity Period: ".$h[scalar($#h)]."###";
    }   
    return $smsp[$counter]=$smsp0."###".$smsp1.$smsp2.$smsp3.$smsp4.$smsp5.$smsp6; 
}
################################################################################################
sub Parse_SMSS{
    
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.102
    #This EF contains status information relating to the short message service.
    
    my @h = @_;    
    
    #the value of the TP-Message-Reference parameter in the last mobile originated short message, as defined
    #in TS 23.040 [13].
    my @t = split(//,(Hex2Bin($h[1])));
    if ($t[7] eq '1'){
        $smss1 = " Flag unset; memory capacity available!";
    }
    if ($t[7] eq '0'){
        $smss1 = " Flag set";
    }
    #This flag is required to allow a process of flow control, so that as memory capacity in the MS becomes
    #available, the Network can be informed. The process for this is described in TS 23.040 [13].
    return $smss = "Last Used TP-MR: ".$h[0]."###".
    "SMS Memory Cap. Exceeded Not. Flag: ".$h[1].$smss1;
}
################################################################################################
sub Parse_LND{
    
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.103
    #This EF contains the last numbers dialled (LND) and/or the respective supplementary service control strings (SSC). In
    #addition it contains identifiers of associated network/bearer capabilities and identifiers of extension records. It may also
    #contain associated alpha-tagging.
    
    my $ref = shift(@_);
    my @h = @$ref;
    my $counter = shift(@_);
    
    my $lnd0 = " ";
    my $lnd1 = " ";
    if ($h[0] eq 'FF'){
        $lnd0="";
        $lnd1="Empty";
    }
    else{
    my @chi;
    my $i=scalar($#h)-13;
    my $a=0;
    for($a=0;$a<$i;$a++){
        $chi[$a]=$h[$a];
    }
    
    my @chibin;
    my @fin;
    for ($ii=0;$ii<$i;$ii++){
        $chibin[$ii]=Hex2Bin($chi[$ii]);
        @fin=(split(//,$chibin[$ii]));
        #Alpha-tagging of the associated dialling number.
        $lnd0 = $lnd0.Trad2Sms(join('',$fin[1],$fin[2],$fin[3],$fin[4],$fin[5],$fin[6],$fin[7]));
    }    
    
    my $c=0;
    for ($c=11;$c>1;$c--){
        $lndp1 = chop($h[scalar($#h)-$c]);
        $lndp2 = ($h[scalar($#h)-$c]);    
        if ($lndp1 eq 'F'){
            $lndp1 = "";
        }
        if ($lndp2 eq 'F'){
            $lndp2 = "";
        }
        $lnd1 =$lnd1.$lndp1.$lndp2;
    }
    }
    return $lnd[$counter] = $lnd0.$lnd1;   
}
################################################################################################
sub Parse_SMSR{
    
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.107
    #This EF contains information in accordance with TS 23.040 [13] comprising short message status reports which have
    #been received by the MS from the network.
    
    my $ref = shift(@_);
    my @h = @$ref;
    my $counter = shift(@_);
    
    my $i = 0;
    $smsr0 = " ";
    for ($i=0;$i<=scalar($#h);$i++){
        $smsr0=$smsr0.$h[$i];
    } 
    return $smsr[$counter]="SMSrecord identifier: ".$h[0]."###".
           "SMSstatus report: ".$smsr0;   
}
################################################################################################
sub Parse_SDN{
    
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.104
    #This EF contains special service numbers (SDN) and/or the respective supplementary service control strings (SSC). In
    #addition it contains identifiers of associated network/bearer capabilities and identifiers of extension records. It may also
    #contain associated alpha-tagging.
    
    my $ref = shift(@_);
    my @h = @$ref;
    my $counter = shift(@_);
    
    my $sdn0 = " ";
    my $sdn1 = " ";
    if ($h[0] eq 'FF'){
        $sdn0="";
        $sdn1="Empty";
    }
    else{
    my @chi;
    my $i=scalar($#h)-13;
    my $a=0;
    for($a=0;$a<$i;$a++){
        $chi[$a]=$h[$a];
    }
    
    my @chibin;
    my @fin;
    for ($ii=0;$ii<$i;$ii++){
        $chibin[$ii]=Hex2Bin($chi[$ii]);
        @fin=(split(//,$chibin[$ii]));
        #Alpha-tagging of the associated dialling number.
        $sdn0 = $sdn0.Trad2Sms(join('',$fin[1],$fin[2],$fin[3],$fin[4],$fin[5],$fin[6],$fin[7]));
    }    
    
    my $c=0;
    for ($c=11;$c>1;$c--){
    $sdnp1=chop($h[scalar($#h)-$c]);
    $sdnp2=($h[scalar($#h)-$c]);
    if ($sdnp1 eq 'F'){
        $sdnp1 = " ";
    }
    if ($sdnp2 eq 'F'){
        $sdnp2 = " ";
    }
        $sdn1=$sdn1.$sdnp1.$sdnp2;
    }
    }
    return $sdn[$counter] = $sdn0.$sdn1; 
}
################################################################################################
sub Parse_EXT1{
    
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.104
    #This EF contains extension data of an ADN/SSC, an MSISDN, or an LND.
    
    my $ref = shift(@_);
    my @h = @$ref;
    my $counter = shift(@_);
    
    #type of the record
    return $ext1[$counter] = "The record tupe is: ".$h[0]."###".
    #Additional data or Called Party Subaddress depending on record type.
    "The Extension data is: ".$h[1].$h[2].$h[3].$h[4].$h[5].$h[6].$h[7].$h[8].$h[9].$h[10].$h[11]."###".
    #identifier of the next extension record to enable storage of information longer than 11 bytes.
    "The Identifier is: ".$h[12];
    #print @ext1;
}
################################################################################################
sub Parse_EXT2{
    
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.106
    #This EF contains extension data of an FDN/SSC
    
    my $ref = shift(@_);
    my @h = @$ref;
    my $counter = shift(@_);
    
    #type of the record
    return $ext2[$counter] = "The record tupe is: ".$h[0]."###".
    #Additional data or Called Party Subaddress depending on record type.
    "The Extension data is: ".$h[1].$h[2].$h[3].$h[4].$h[5].$h[6].$h[7].$h[8].$h[9].$h[10].$h[11]."###".
    #identifier of the next extension record to enable storage of information longer than 11 bytes.
    "The Identifier is: ".$h[12];
    #print @ext2;
}
################################################################################################
sub Parse_EXT3{
    
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.106
    #This EF contains extension data of an SDN
    
    my $ref = shift(@_);
    my @h = @$ref;
    my $counter = shift(@_);
    
    #type of the record
    return $ext3[$counter] = "The record tupe is: ".$h[0]."###".
    #Additional data or Called Party Subaddress depending on record type.
    "The Extension data is: ".$h[1].$h[2].$h[3].$h[4].$h[5].$h[6].$h[7].$h[8].$h[9].$h[10].$h[11]."###".
    #identifier of the next extension record to enable storage of information longer than 11 bytes.
    "The Identifier is: ".$h[12];
    #print @ext3;
}
################################################################################################
sub Parse_BDN{
    
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.106
    #This EF contains Barred Dialling Numbers (BDN) and/or Supplementary Service Control strings (SSC). In addition it
    #contains identifiers of associated network/bearer capabilities and identifiers of extension records. It may also contain an
    #associated alpha-tagging.
    
    my $ref = shift(@_);
    my @h = @$ref;
    my $counter = shift(@_);
    
    my @chi;
    my $i=scalar($#h)-14;
    my $a=0;
    for($a=0;$a<$i;$a++){
        $chi[$a]=$h[$a];
    }
    $bdn0 = " ";
    my @chibin;
    my @fin;
    for ($ii=0;$ii<$i;$ii++){
        $chibin[$ii]=Hex2Bin($chi[$ii]);
        @fin=(split(//,$chibin[$ii]));
        #Alpha-tagging of the associated dialling number.
        $bdn0 = $bdn0.Trad2Sms(join('',$fin[1],$fin[2],$fin[3],$fin[4],$fin[5],$fin[6],$fin[7]));
    }    
    my $bdn1 = " ";
    my $c=0;
    for ($c=11;$c>1;$c--){
        $bdnp1 = chop($h[scalar($#h)-$c]);
        $bdnp2 = ($h[scalar($#h)-$c]);    
        if ($bdnp1 eq 'F'){
            $bdnp1 = "";
        }
        if ($bdnp2 eq 'F'){
            $bdnp2 = "";
        }
        $bdn1 =$bdn1.$bdnp1.$bdnp2;
    } 
    return $bdn[$counter] = $bdn0.$bdn1;    
}
################################################################################################
sub Parse_EXT4{
    
    #ETSI TS 100 977 V8.13.0 (2005-06) pag.107
    #This EF contains extension data of an BDN/SSC
    
    my $ref = shift(@_);
    my @h = @$ref;
    my $counter = shift(@_);
    
    #type of the record
    return $ext4[$counter] = "The record tupe is: ". 
    #Additional data or Called Party Subaddress depending on record type.
    "The Extension data is: ".$h[1].$h[2].$h[3].$h[4].$h[5].$h[6].$h[7].$h[8].$h[9].$h[10].$h[11]."###".
    #identifier of the next extension record to enable storage of information longer than 11 bytes.
    "The Identifier is: ".$h[12];
    #print @ext4;
}
###################################################################################################

my $ref = {
'MF' =>                                       
        {# MF or root of the hash
         
        'header' => $df_header{"MF"},
        'EF' => 
            [
            { 'ICCID' => [{
                            'header' => $ef_header{"ICCID"},
                            'description' => $ef_description{"ICCID"},
                            'content' => [$iccid],
                         },],
            },
            {   'ELP' => [{
                            'header' => $ef_header{"ELP"},
                            'description' => $ef_description{"ELP"},
                            'content' => [$elp],
                         },],
            },                           
            ],
        
        'DF_7F20' => 
            [                                                                                                                                                                                     
            { 
             'header' => $df_header{"DFGSM"},
             'EF' => 
                    [                                               
                    {  'LP' => [{
                                  'header'  => $ef_header{"LP"}, 
                                  'description' => $ef_description{"LP"},                               
                                  'content' => [$lp], 
                                  },],                                            
                    },
                    {  'IMSI' => [{
                                  'header'  => $ef_header{"IMSI"},  
                                  'description' => $ef_description{"IMSI"},                              
                                  'content' => [$imsi], 
                                  },],                                            
                    },
                    {  'KC' => [{
                                  'header'  => $ef_header{"KC"},
                                  'description' => $ef_description{"KC"},                                
                                  'content' => [$kc], 
                                  },],                                            
                    },
                    {  'PLMNsel' => [{
                                  'header'  => $ef_header{"PLMNsel"},
                                  'description' => $ef_description{"PLMNsel"},                                
                                  'content' => [$plmnsel], 
                                  },],                                            
                    },
                    {  'HPLMN' => [{
                                  'header'  => $ef_header{"HPLMN"},                            
                                  'description' => $ef_description{"HPLMN"},    
                                  'content' => [$hplmn], 
                                  },],                                            
                    },
                    {  'ACMmax' => [{
                                  'header'  => $ef_header{"ACMmax"},                                
                                  'description' => $ef_description{"ACMmax"},
                                  'content' => [$acmmax], 
                                  },],                                            
                    },
                    {  'SST' => [{
                                  'header'  => $ef_header{"SST"},                                
                                  'description' => $ef_description{"SST"},
                                  'content' => [$sst], 
                                  },],                                            
                    },
                    {  'ACM' => [{
                                  'header'  => $ef_header{"ACM"},
                                  'description' => $ef_description{"ACM"},                                
                                  'content' => [@acm], 
                                  },],                                            
                    },
                    {  'GID1' => [{
                                  'header'  => $ef_header{"GID1"},                                
                                  'description' => $ef_description{"GID1"},
                                  'content' => [$gid1], 
                                  },],                                            
                    },
                    {  'GID2' => [{
                                  'header'  => $ef_header{"GID2"},                                
                                  'description' => $ef_description{"GID2"},
                                  'content' => [$gid2], 
                                  },],                                            
                    },
                    {  'PUCT' => [{
                                  'header'  => $ef_header{"PUCT"},                                
                                  'description' => $ef_description{"PUCT"},
                                  'content' => [$puct], 
                                  },],                                            
                    },
                    {  'CBMI' => [{
                                  'header'  => $ef_header{"CBMI"},                                
                                  'description' => $ef_description{"CBMI"},
                                  'content' => [$cbmi], 
                                  },],                                            
                    },
                    {  'SPN' => [{
                                  'header'  => $ef_header{"SPN"},                                
                                  'description' => $ef_description{"SPN"},
                                  'content' => [$spn], 
                                  },],                                            
                    },
                    {  'CBMID' => [{
                                  'header'  => $ef_header{"CBMID"},                                
                                  'description' => $ef_description{"CBMID"},
                                  'content' => [$cbmid], 
                                  },],                                            
                    },
                    {  'BCCH' => [{
                                  'header'  => $ef_header{"BCCH"},                                
                                  'description' => $ef_description{"BCCH"},
                                  'content' => [$bcch], 
                                  },],                                            
                    },
                    {  'ACC' => [{
                                  'header'  => $ef_header{"ACC"},                                
                                  'description' => $ef_description{"ACC"},
                                  'content' => [$acc], 
                                  },],                                            
                    },
                    {  'FPLMN' => [{
                                  'header'  => $ef_header{"FPLMN"},                                
                                  'description' => $ef_description{"FPLMN"},
                                  'content' => [$fplmn], 
                                  },],  
                    },
                    {  'LOCI' => [{
                                  'header'  => $ef_header{"LOCI"},                                
                                  'description' => $ef_description{"LOCI"},
                                  'content' => [$loci], 
                                  },],                                            
                    },
                    {  'AD' => [{
                                  'header'  => $ef_header{"AD"},                                
                                  'description' => $ef_description{"AD"},
                                  'content' => [$ad], 
                                  },],                                            
                    },
                    {  'PHASE' => [{
                                  'header'  => $ef_header{"PHASE"},                                
                                  'description' => $ef_description{"PHASE"},
                                  'content' => [$phase], 
                                  },],                                            
                    },
                    {  'VGCS' => [{
                                  'header'  => $ef_header{"VGCS"},                                
                                  'description' => $ef_description{"VGCS"},
                                  'content' => [$vgcs], 
                                  },],                                            
                    },
                    {  'VGCSS' => [{
                                  'header'  => $ef_header{"VGCSS"},                                
                                  'description' => $ef_description{"VGCSS"},
                                  'content' => [$vgcss], 
                                  },],                                            
                    },
                    {  'VBS' => [{
                                  'header'  => $ef_header{"VBS"},                                
                                  'description' => $ef_description{"VBS"},
                                  'content' => [$vbs], 
                                  },],                                            
                    },
                    {  'VBSS' => [{
                                  'header'  => $ef_header{"VBSS"},                                
                                  'description' => $ef_description{"VBSS"},
                                  'content' => [$vbss], 
                                  },],                                            
                    },
                    {  'eMLPP' => [{
                                  'header'  => $ef_header{"eMLPP"},                                
                                  'description' => $ef_description{"eMLPP"},
                                  'content' => [$emlpp], 
                                  },],                                            
                    },
                    {  'AAeM' => [{
                                  'header'  => $ef_header{"AAeM"},                                
                                  'description' => $ef_description{"AAeM"},
                                  'content' => [$aaem], 
                                  },],                                            
                    },
                    {  'ECC' => [{
                                  'header'  => $ef_header{"ECC"},                                
                                  'description' => $ef_description{"ECC"},
                                  'content' => [$ecc], 
                                  },],                                            
                    },
                    {  'CBMIR' => [{
                                  'header'  => $ef_header{"CBMIR"},                                
                                  'description' => $ef_description{"CBMIR"},
                                  'content' => [$cbmir], 
                                  },],                                            
                    },
                    {  'NIA' => [{
                                  'header'  => $ef_header{"NIA"},                                
                                  'description' => $ef_description{"NIA"},
                                  'content' => [@nia], 
                                  },],
                    },
                    {  'KcGPRS' => [{
                                  'header'  => $ef_header{"KCGPRS"},                                
                                  'description' => $ef_description{"KcGPRS"},
                                  'content' => [$kcgprs], 
                                  },],                                            
                    },
                    {  'LOCIGPRS' => [{
                                  'header'  => $ef_header{"LOCIGPRS"},                                
                                  'description' => $ef_description{"LOCIGPRS"},
                                  'content' => [$locigprs], 
                                  },],                                            
                    },
                    {  'SUME' => [{
                                  'header'  => $ef_header{"SUME"},                                
                                  'description' => $ef_description{"SUME"},
                                  'content' => [$sume], 
                                  },],                                            
                    },
                    {  'PLMNwACT' => [{
                                  'header'  => $ef_header{"PLMNwACT"},                                
                                  'description' => $ef_description{"PLMNwACT"},
                                  'content' => [$plmnwact], 
                                  },],                                            
                    },
                    {  'OPLMNwACT' => [{
                                  'header'  => $ef_header{"OPLMNwACT"},                                
                                  'description' => $ef_description{"OPLMNwACT"},
                                  'content' => [$oplmnwact], 
                                  },],                                            
                    },
                    {  'HPLMNwACT' => [{
                                  'header'  => $ef_header{"HPLMNwACT"},                                
                                  'description' => $ef_description{"HPLMNwACT"},
                                  'content' => [$hplmnwact], 
                                  },],                                            
                    },
                    {  'CPBCCH' => [{
                                  'header'  => $ef_header{"CPBCCH"},
                                  'description' => $ef_description{"CPBCCH"},                                
                                  'content' => [$cpbcch], 
                                  },],                                            
                    },
                    {  'INVSCAN' => [{
                                  'header'  => $ef_header{"INVSCAN"},   
                                  'description' => $ef_description{"INVSCAN"},                             
                                  'content' => [$invscan], 
                                  },],                                            
                    },                                   
                    ],
                                        
                    'DF_5F3C' =>
                            [
                            {
                            'header' => $df_header{"DFMExE"},
                            'EF' =>
                                    [
                                    { 'MExEST' => [{
                                                    'header' => $ef_header{"MExEST"},
                                                    'description' => $ef_description{"MExEST"},
                                                    'content' => [$mexest],
                                                   },],
                                    },
                                    {'ORPK' => [{
                                                    'header' => $ef_header{"ORPK"},
                                                    'description' => $ef_description{"ORPK"},
                                                    'content' => [@orpk],
                                                },],
                                    },
                                    {'ARPK' => [{
                                                    'header' => $ef_header{"ARPK"},
                                                    'description' => $ef_description{"ARPK"},
                                                    'content' => [@arpk],
                                                },],
                                    },
                                    {'TPRPK' => [{
                                                    'header' => $ef_header{"TPRPK"},
                                                    'description' => $ef_description{"TPRPK"},
                                                    'content' => [@trpk],
                                                },],                 
                                    },
                                    ],                                          
                            },
                            ],
                            
                    'DF_5F70' =>
                            [
                            {
                            'header' => $df_header{"DFSoLSA"},
                            'EF' =>
                                    [
                                    { 'SAI' => [{
                                                    'header' => $ef_header{"SAI"},
                                                    'description' => $ef_description{"SAI"},
                                                    'content' => [$sai],
                                                   },],
                                    },
                                    {'SLL' => [{
                                                    'header' => $ef_header{"SLL"},
                                                    'description' => $ef_description{"SLL"},
                                                    'content' => [@sll],
                                                },],
                                    },
                                    ],                                          
                            },
                            ],
                            
                            
                            
            },
            ], # DF 7F20
            
        'DF_7F10' =>                                                    
            [                                                                             
            {
             'header' => $df_header{"DFTELECOM"},
             'EF' => 
                    [ 
                    { 'ADN' => [{
                                  'header'  => $ef_header{"ADN"},
                                  'description' => $ef_description{"ADN"},                                
                                  'content' => [@adn], 
                                 },],                                            
                    },
                    { 'FDN' => [{
                                  'header'  => $ef_header{"FDN"},
                                  'description' => $ef_description{"FDN"},                                
                                  'content' => [@fdn], 
                                 },],                                            
                    },
                    {  'SMS' => [{
                                  'header'  => $ef_header{"SMS"},
                                  'description' => $ef_description{"SMS"},                                
                                  'content' => [@sms], 
                                  },],                                            
                    },
                    {  'CCP' => [{
                                  'header'  => $ef_header{"CCP"},
                                  'description' => $ef_description{"CCP"},                                
                                  'content' => [@ccp], 
                                  },],                                            
                    },
                    {  'MSISDN' => [{
                                  'header'  => $ef_header{"MSISDN"},
                                  'description' => $ef_description{"MSISDN"},                                
                                  'content' => [@msisdn], 
                                  },],                                            
                    },
                    {  'SMSP' => [{
                                  'header'  => $ef_header{"SMSP"},
                                  'description' => $ef_description{"SMSP"},                                
                                  'content' => [@smsp], 
                                  },],                                            
                    },
                    {  'SMSS' => [{
                                  'header'  => $ef_header{"SMSS"},
                                  'description' => $ef_description{"SMSS"},                                
                                  'content' => [$smss], 
                                  },],                                            
                    },
                    {  'LND' => [{
                                  'header'  => $ef_header{"LND"},
                                  'description' => $ef_description{"LND"},                                
                                  'content' => [@lnd], 
                                  },],                                            
                    },
                    {  'SMSR' => [{
                                  'header'  => $ef_header{"SMSR"},
                                  'description' => $ef_description{"SMSR"},                                
                                  'content' => [@smsr], 
                                  },],                                            
                    },
                    {  'SDN' => [{
                                  'header'  => $ef_header{"SDN"},
                                  'description' => $ef_description{"SDN"},                                
                                  'content' => [@sdn], 
                                  },],                                            
                    },                                              
                    {  'EXT1' => [{
                                  'header'  => $ef_header{"EXT1"},
                                  'description' => $ef_description{"EXT1"},                                
                                  'content' => [@ext1], 
                                  },],                                            
                    },
                    {  'EXT2' => [{
                                  'header'  => $ef_header{"EXT2"},                                
                                  'description' => $ef_description{"EXT2"},
                                  'content' => [@ext2], 
                                  },],                                            
                    },
                    {  'EXT3' => [{
                                  'header'  => $ef_header{"EXT3"},                                
                                  'description' => $ef_description{"EXT3"},
                                  'content' => [@ext3], 
                                  },],                                            
                    },
                    {  'BDN' => [{
                                  'header'  => $ef_header{"BDN"},                                
                                  'description' => $ef_description{"BDN"},
                                  'content' => [@bdn], 
                                  },],                                            
                    },
                    {  'EXT4' => [{
                                  'header'  => $ef_header{"EXT4"},                                
                                  'description' => $ef_description{"EXT4"},
                                  'content' => [@ext4], 
                                  },],                                            
                    },                                                                                                                                           
                    ],                                              
            },                                                      
            ],
        
        
        }# MF                                                
           
                                         
};



my $simple2 = XML::Simple->new();                                               
my $tree2 = $simple2->XMLout($ref);

# Output File   
my @in = split(/\./, $file_in);
my $file_out = "$in[0]".'.out.xml';
open OUT, ">$file_out";                                                                                
my $header = '<?xml version="1.0" encoding="ISO-8859-1"?>'."\n".'<?xml-stylesheet type="text/xsl" href="template.xml"?>'."\n";
print OUT $header;
print OUT $tree2;
`rm -f tmp.xml`;
printf "Report $file_out has ben succesfully generated\n";
close OUT;

##########################################################################################
### Hex2Bin 
##########################################################################################
sub Hex2Bin {
    my $hex = $_[0];
    my %h2b = ( 0 => "0000", 1 => "0001", 2 => "0010", 3 => "0011",
             4 => "0100", 5 => "0101", 6 => "0110", 7 => "0111",   
             8 => "1000", 9 => "1001", a => "1010", b => "1011",
             c => "1100", d => "1101", e => "1110", f => "1111");
    (my $binary = $hex) =~ s/(.)/$h2b{lc $1}/g;
    return $binary;
}

##########################################################################################
### Hex2Dec
##########################################################################################
sub Hex2Dec {
    my $dec = $_[0];
    my %ccc = ( 0 => "0", 1 => "1", 2 => "2", 3 => "3",
             4 => "4", 5 => "5", 6 => "6", 7 => "7",   
             8 => "8", 9 => "9", a => "10", b => "11",
             c => "12", d => "13", e => "14", f => "15");
    (my $fin = $dec) =~ s/(.)/$ccc{lc $1}/g;
    return $fin;
}

##########################################################################################
### SmsTraduction 
##########################################################################################
sub Trad2Sms  {
   my $tr = shift(@_);
   my %s2s = (
             '0000000' => '@', # number 0
             '0000001' => '£',
             '0000010' => '$',
             '0000011' => '¥',
             '0000100' => 'è', 
             '0000101' => 'é',
             '0000110' => 'ù',
             '0000111' => 'ì',
             '0001000' => 'ò',
             '0001001' => 'Ç',
             '0001010' => 'Ø',
             '0001011' => 'ø',
             '0001100' => 'Å',
             '0001101' => 'å', 
             '0001110' => ' ', # not representable
             '0001111' => '_',
             '0010000' => ' ',
             '0010001' => ' ',
             '0010010' => "\n",
             '0010011' => '^',
             '0010100' => '}',
             '0010101' => '{',
             '0010110' => '\\',
             '0010111' => ']',
             '0011000' => '~',
             '0011001' => '[',
             '0011010' => '|', 
             '0011011' => '€',
             '0011100' => 'Æ',
             '0011101' => 'æ',
             '0011110' => 'ß',
             '0011111' => 'É',
             '0100000' => ' ', # space
             '0100001' => '!',
             '0100010' => '"',
             '0100011' => '#',
             '0100100' => '¤',
             '0100101' => '%',
             '0100110' => '&',
             '0100111' => "'",
             '0101000' => '(',
             '0101001' => ')',
             '0101010' => '*',
             '0101011' => '+',
             '0101100' => ',',
             '0101101' => '-',
             '0101110' => '.',
             '0101111' => '/',
             '0110000' => '0',
             '0110001' => '1',
             '0110010' => '2',
             '0110011' => '3',
             '0110100' => '4',
             '0110101' => '5',
             '0110110' => '6',
             '0110111' => '7',
             '0111000' => '8',
             '0111001' => '9',
             '0111010' => ':',
             '0111011' => ';',
             '0111100' => '<',
             '0111101' => '=',
             '0111110' => '>',
             '0111111' => '?',
             '1000000' => '¡',
             '1000001' => 'A',
             '1000010' => 'B',
             '1000011' => 'C',
             '1000100' => 'D',
             '1000101' => 'E', 
             '1000110' => 'F',
             '1000111' => 'G',
             '1001000' => 'H',
             '1001001' => 'I',
             '1001010' => 'J',
             '1001011' => 'K',
             '1001100' => 'L',
             '1001101' => 'M',
             '1001110' => 'N',
             '1001111' => 'O',
             '1010000' => 'P', 
             '1010001' => 'Q',
             '1010010' => 'R',
             '1010011' => 'S',
             '1010100' => 'T',
             '1010101' => 'U',
             '1010110' => 'V',
             '1010111' => 'W',
             '1011000' => 'X',
             '1011001' => 'Y',
             '1011010' => 'Z', 
             '1011011' => 'Ä',
             '1011100' => 'Ö',
             '1011101' => 'Ñ',
             '1011110' => 'Ü',
             '1011111' => '§',
             '1100000' => ' ', #¿
             '1100001' => 'a',
             '1100010' => 'b',
             '1100011' => 'c', 
             '1100100' => 'd',
             '1100101' => 'e',
             '1100110' => 'f',
             '1100111' => 'g',
             '1101000' => 'h',
             '1101001' => 'i',
             '1101010' => 'j',
             '1101011' => 'k',
             '1101100' => 'l',
             '1101101' => 'm',
             '1101110' => 'n',
             '1101111' => 'o',
             '1110000' => 'p',
             '1110001' => 'q',
             '1110010' => 'r',
             '1110011' => 's',
             '1110100' => 't',
             '1110101' => 'u',
             '1110110' => 'v',
             '1110111' => 'w',
             '1111000' => 'x',
             '1111001' => 'y',
             '1111010' => 'z',
             '1111011' => 'ä',
             '1111100' => 'ö',
             '1111101' => 'ñ',
             '1111110' => 'ü',
             '1111111' => ' '); #à
   

    (my $traduction = $tr) =~ s/(.......)/$s2s{lc $1}/;
    return $traduction;
 }   
##########################################################################################
### SmsTraduction stop
##########################################################################################

# Perl trim function to remove whitespace from the start and end of the string
sub trim($)
{
	my $string = shift;
	$string =~ s/^\s+//;
	$string =~ s/\s+$//;
	return $string;
}
# Left trim function to remove leading whitespace
sub ltrim($)
{
	my $string = shift;
	$string =~ s/^\s+//;
	return $string;
}
# Right trim function to remove trailing whitespace
sub rtrim($)
{
	my $string = shift;
	$string =~ s/\s+$//;
	return $string;
}
