package com.zweitgeist.tools.langman.data;

import java.io.File;
import java.io.FileInputStream;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Properties;
import java.util.Vector;

import com.zweitgeist.tools.langman.Constants;
import com.zweitgeist.tools.langman.LangMan;
import com.zweitgeist.tools.langman.Tools;

/**
 * The default implementation of the Dictionary interface. Works with a csv file
 * @author jkuehne
 *
 */
public class DefaultDictionary implements Dictionary, Constants{
	
	private LangMan					parent;
	private Vector<Language>		vLanguages;
	private Vector<String> 			vParts;
	private Vector<DictionaryItem>	vItems;
	private Language 				defaultLang;
	private String[]				saLayout;
	private String					sEmptyString;
	private Properties				prpSettings;
	
	// Statistics
	private Vector<DictionaryEntry>	vDefaultReplacements;
	private Vector<DictionaryEntry>	vKeysWithoutVals;
	
	public DefaultDictionary(File fContent, File fSettings, String sDefaultLang, String sEmptyString, LangMan parent, boolean bHideImportWarnings){
		this.parent = parent;
		this.sEmptyString = sEmptyString;
		if (fSettings != null){
			try{
				this.prpSettings = new Properties();
				this.prpSettings.load(new FileInputStream(fSettings));
			} catch ( Exception e ){
				parent.printError( "Could not load properties '"+fSettings+"' for dictionary!: "+e );
			}
		}
		String[][] saCsv = Tools.readCsv(fContent, "UTF-8");
		if (saCsv != null){
			init(saCsv, sDefaultLang, sEmptyString, bHideImportWarnings);
		} else {
			parent.printError("CSV file '"+fContent+"' could not be parsed - check the log!");
		}
	}
	
	
	
	public void resetStatistics(){
		this.vDefaultReplacements = new Vector<DictionaryEntry>();
		this.vKeysWithoutVals = new Vector<DictionaryEntry>();
	}
	
	
	
	public DictionaryEntry[] getDefaultReplacements(){
		return vDefaultReplacements.toArray(new DictionaryEntry[vDefaultReplacements.size()]);
	}
	
	
	
	public DictionaryEntry[] getKeysWithoutVals(){
		return vKeysWithoutVals.toArray(new DictionaryEntry[vKeysWithoutVals.size()]);
	}
	
	
	
	public DictionaryEntry[] getStatistics(int nIndex){
		switch (nIndex){
			case ST_DEFAULT_REPLACEMENTS: 	return vDefaultReplacements.toArray(new DictionaryEntry[vDefaultReplacements.size()]);
			case ST_KEYS_WITHOUT_VALS: 		return vKeysWithoutVals.toArray(new DictionaryEntry[vKeysWithoutVals.size()]);
			default: return null;
		}
		
	}
	
	
	
	/**
	 * Called by DictionaryEntries in case they used the default language for a translation because
	 * the desired language was not available.
	 * @param entry The entry that used the default language
	 */
	public void callBackDefaultReplacementUsed(DictionaryEntry entry){
		this.vDefaultReplacements.add(entry);
	}
	
	
	
	/**
	 * Called by DictionaryEntries in case they used no key for a translation because
	 * the desired language was not available and the default language was not available as well or 
	 * should not be used.
	 * @param entry The entry that used the default language
	 */
	public void callBackKeyWithoutValUsed(DictionaryEntry entry){
		this.vKeysWithoutVals.add(entry);
	}
	
	
	
	public boolean contains(Language lang){
		boolean result = this.vLanguages.contains(lang);
		return result;
	}
	
	
	
	public boolean contains(DictionaryEntry entry){
		return this.vItems.contains(entry);
	}
	
	
	
	public boolean containsPart(String part){
		return this.vParts.contains(part);
	}
	
	
	
	public Language getDefaultLang(){
		return this.defaultLang;
	}
	
	
	
	public String[] getParts(){
		return this.vParts.toArray(new String[this.vParts.size()]);
	}
	
	
	public Language getLanguage(String sLang){
		Language result = null;
		
		Language tempLang = new Language(sLang);
		int nIndex = this.vLanguages.indexOf(tempLang);		
		if (nIndex >= 0){
			result = this.vLanguages.get(nIndex);			
		}
		return result;
	}
	
	
	public Language[] getLanguages(){
		return this.vLanguages.toArray(new Language[this.vLanguages.size()]);
	}
	
	
	
	private boolean init(String[][] saCsv, String sDefaultLang, String sEmptyString, boolean bHideImportWarnings){
		resetStatistics();
		return createInternalData(saCsv, sDefaultLang, sEmptyString, bHideImportWarnings, false);
	}
	
	
	
	public boolean loadPatch(File file, boolean bHideImportWarnings){
		String[][] saCsv = Tools.readCsv(file, "UTF-8");
		if (saCsv != null){
			return createInternalData(saCsv, null, this.sEmptyString, bHideImportWarnings, true);
		} else {
			return false;
		}
	}
	
	
	
	public boolean exportToCsv(File file){
		if (!file.exists()){
			try{
				file.createNewFile();
			} catch (Exception e){
				parent.printError("An exception was thrown when I tried to create '"+file+"': "+e);
				return false;
			}
		}
		StringBuffer sbCsv = new StringBuffer();
		DictionaryItem[] items = this.vItems.toArray(new DictionaryItem[this.vItems.size()]);
		for (int i = 0; i < items.length; i++) {
			if (items[i].isDictionaryComment()){
				DictionaryComment comment = (DictionaryComment)items[i];
				String[] saComment = comment.getContent();
				for (int j = 0; j < saComment.length; j++) {
					sbCsv.append(Tools.encodeCsv(saComment[j]));
					if (j < saComment.length-1){
						sbCsv.append(";");
					}
				}
				if (comment.isDuplicateKey()){
					parent.printWarning("Exported duplicate key in line "+comment.getLineNo());
				}
			} else {
				DictionaryEntry entry = (DictionaryEntry)items[i];
				for (int j = 0; j < this.saLayout.length; j++) {
					String sFieldVal = "";
					// Take care of the must-have columns					
//					if (this.saLayout[j].equals(S_CLOLUMN_NAMES[CLM_NUMBER])){
//						sFieldVal = ""+entry.getNumber();
//					} else 
					if (this.saLayout[j].equals(S_CLOLUMN_NAMES[CLM_PART])){
						sFieldVal = entry.getPart();
					} else if (this.saLayout[j].equals(S_CLOLUMN_NAMES[CLM_CHANGED])){
						if (entry.getHasChanged()){
							sFieldVal = "1";
						}
					} else if (this.saLayout[j].equals(S_CLOLUMN_NAMES[CLM_KEY])){
						sFieldVal = entry.getKey();
					} else if (this.saLayout[j].equals(S_CLOLUMN_NAMES[CLM_REFERENCE])){
						sFieldVal = entry.getSReference();
					} else if (this.saLayout[j].equals(S_CLOLUMN_NAMES[CLM_COMMENT])){
						sFieldVal = entry.getComment();
					} else if (this.saLayout[j].equals(S_CLOLUMN_NAMES[CLM_LIMIT])){
						if (entry.getLimit() > 0)
							sFieldVal += entry.getLimit();
						
					// Take care of the languages
					} else {
						if (entry.getReference() == null) {
							Enumeration<Language> enumLangs = this.vLanguages.elements();
							boolean bFound = false;
							while (enumLangs.hasMoreElements() && !bFound){
								Language currentLang = enumLangs.nextElement();
								if (this.saLayout[j].equalsIgnoreCase(currentLang.getShortName())){
									sFieldVal = entry.getValue(currentLang, false);
									if (sFieldVal == null){
										sFieldVal = "";
									} else if (sFieldVal.equals("")){
										sFieldVal = sEmptyString;
									}
									bFound = true;
								}
							}
						}
					}
					sbCsv.append(Tools.encodeCsv(sFieldVal));
					if (j < saLayout.length-1){
						sbCsv.append(";");
					}
				}
			}
			sbCsv.append("\r\n");
		}
		try {
			Tools.writeTextFile(file, sbCsv.toString(), "UTF-8");
		} catch (Exception e){
			parent.printError("An exception was thrown when I tried to write the csv to file '"+file+"': "+e);
			return false;
		}
		return true;
	}
	
	
	
	public boolean generateTranslatorFile(File file, Language target, Language[] sources, String[] parts){
		if (!file.exists()){
			try{
				file.createNewFile();
			} catch (Exception e){
				parent.printError("An exception was thrown when I tried to create '"+file+"': "+e);
				return false;
			}
		}
		
		// Init the layout of the translator file
		String[] saTranslatorLayout = new String[5+sources.length];
		saTranslatorLayout[0] = S_CLOLUMN_NAMES[CLM_PART];
		saTranslatorLayout[1] = S_CLOLUMN_NAMES[CLM_KEY];
		saTranslatorLayout[2] = S_CLOLUMN_NAMES[CLM_COMMENT];
		saTranslatorLayout[3] = S_CLOLUMN_NAMES[CLM_LIMIT];
		for (int i = 0; i < sources.length; i++) {
			saTranslatorLayout[4+i] = sources[i].getShortName();
		}
		saTranslatorLayout[saTranslatorLayout.length-1] = target.getShortName();
		
		
		StringBuffer sbCsv = new StringBuffer();
		DictionaryItem[] items = this.vItems.toArray(new DictionaryItem[this.vItems.size()]);
		for (int i = 0; i < saTranslatorLayout.length; i++) {
			sbCsv.append(Tools.encodeCsv(saTranslatorLayout[i]));
			if (i < saTranslatorLayout.length-1){
				sbCsv.append(";");
			}
		}
		sbCsv.append("\r\n");
		
		// Put into vector for convenient check later on (vParts.contains)
		Vector<String> vParts = new Vector<String>(parts.length);
		for (int i = 0; i<parts.length; i++){
			vParts.add( parts[i] );
		}
		boolean bSourceInvalid = false;
		for (int i = 0; i < items.length; i++) {	
			if (items[i].isDictionaryComment()){
				DictionaryComment comment = (DictionaryComment)items[i];
				if (comment.isDuplicateKey()){					
					String[] saComment = comment.getContent();
					for (int j = 0; j < saComment.length; j++) {
						sbCsv.append(Tools.encodeCsv(saComment[j]));
						if (j < saComment.length-1){
							sbCsv.append(";");
						}
					}
					sbCsv.append("\r\n");
					parent.printWarning("Exported duplicate key in line "+comment.getLineNo());
				}
			} else {
				DictionaryEntry entry = (DictionaryEntry)items[i];
				if ( entry.getReference() == null && ((entry.getValue(target, false) == null || entry.getHasChanged()) && vParts.contains(entry.getPart())) ){
					for (int j = 0; j < saTranslatorLayout.length; j++) {
						String sFieldVal = "";
						// Take care of the must-have columns					 
						if (saTranslatorLayout[j].equals(S_CLOLUMN_NAMES[CLM_PART])){
							sFieldVal = entry.getPart();
						} else if (saTranslatorLayout[j].equals(S_CLOLUMN_NAMES[CLM_CHANGED])){
							if (entry.getHasChanged()){
								sFieldVal = "1";
							}
						} else if (saTranslatorLayout[j].equals(S_CLOLUMN_NAMES[CLM_KEY])){
							sFieldVal = entry.getKey();
						} else if (saTranslatorLayout[j].equals(S_CLOLUMN_NAMES[CLM_COMMENT])){
							sFieldVal = entry.getComment();
						} else if (saTranslatorLayout[j].equals(S_CLOLUMN_NAMES[CLM_LIMIT])){
							if (entry.getLimit() > 0)
								sFieldVal += entry.getLimit();
							
						// Take care of the languages
						} else {
							Enumeration<Language> enumLangs = this.vLanguages.elements();
							boolean bFound = false;
							while (enumLangs.hasMoreElements() && !bFound){
								Language currentLang = enumLangs.nextElement();
								if (saTranslatorLayout[j].equalsIgnoreCase(currentLang.getShortName())){
									sFieldVal = entry.getValue(currentLang, false);
									if (sFieldVal == null && currentLang != target){
										bSourceInvalid = true;										
									}
									if (sFieldVal == null){
										sFieldVal = "";
									} else if (sFieldVal.equals("")){
										sFieldVal = sEmptyString;
									}
									bFound = true;
								}
							}
						}
						sbCsv.append(Tools.encodeCsv(sFieldVal));
						if (j < saLayout.length-1){
							sbCsv.append(";");
						}
					}
					sbCsv.append("\r\n");
				}
			}			
		}
		if (bSourceInvalid){
			parent.printWarning( "One of the target languages should not be used as source for target '"+target+"', because it does not contain all texts needed for translation!");
		}
		try {
			Tools.writeTextFile(file, sbCsv.toString(), "UTF-8");
		} catch (Exception e){
			parent.printError("An exception was thrown when I tried to write the csv to file '"+file+"': "+e);
			return false;
		}
		return true;
	}
	
	
	
	public String getSettingProperty(String sKey, boolean bVerbose){
		String sVal = null;
		if (this.prpSettings != null){
			sVal = this.prpSettings.getProperty(sKey);
		}
		if (sVal == null && bVerbose){
			parent.printError("Could not find a val for key '"+sKey+"' in the config file!");
		}
		return sVal;
	}
		
	
	
	private boolean createInternalData(String[][] saCsv, String sDefaultLang, String sEmptyString, boolean bHideImportWarnings, boolean bIsPatch){
		boolean bOk = true;
		Vector<Language> vImportLanguages;		
		if (bIsPatch){
			vImportLanguages = new Vector<Language>();			
		} else {
			this.vLanguages		= new Vector<Language>();
			this.vItems			= new Vector<DictionaryItem>();
			this.vParts			= new Vector<String>();
			vImportLanguages 	= this.vLanguages;
			
		}
		boolean bIsInitialized = false;
		int[] nColumnNumbers = new int[S_CLOLUMN_NAMES.length];
		for( int i=0; i<nColumnNumbers.length; i++){
			nColumnNumbers[i] = -1;
		}
		HashMap<Integer, Language> hmColumnIndexToLang = new HashMap<Integer, Language>();
		Vector<DictionaryEntry> vEntriesWithReferences = new Vector<DictionaryEntry>();
		
		for (int nLineNo = 1; nLineNo <= saCsv.length; nLineNo++){
			String[] saCurrentRow = saCsv[nLineNo-1];
			if (saCurrentRow == null){
				parent.printError( "There is something really wrong with the csv you are trying to load!");
				bOk = false;
				break;
			} else {
				if (
					saCurrentRow.length > 0 &&
					saCurrentRow[0] != null && 
					!saCurrentRow[0].startsWith( "#" ) && 
					!saCurrentRow[0].startsWith( "//" ) &&
					!saCurrentRow[0].startsWith( "\"#" ) && 
					!saCurrentRow[0].startsWith( "\"//" ) 
					){
					if (!bIsInitialized){
						for (int nCurrentColumn=0; nCurrentColumn<saCurrentRow.length; nCurrentColumn++){
							boolean bfoundKnownColumnName = false;
							String sCurrentField = saCurrentRow[nCurrentColumn]; 
								
							for(int i=0; i<S_CLOLUMN_NAMES.length && !bfoundKnownColumnName; i++){									
								if (sCurrentField != null && sCurrentField.equalsIgnoreCase(S_CLOLUMN_NAMES[i])){
									nColumnNumbers[i] = nCurrentColumn;
									bfoundKnownColumnName = true;
								}
							}
							if(!bfoundKnownColumnName && sCurrentField != null && !sCurrentField.trim().equals("")){
								Language langNew = new Language(sCurrentField);
								if (vImportLanguages.contains(langNew)) {
									parent.printWarning("Language '"+langNew+"' was found more than once - only the first occurance will be used!");
								} else {
									if ( sDefaultLang != null && sCurrentField.equalsIgnoreCase(sDefaultLang)){
										this.defaultLang = langNew;
									}
									vImportLanguages.add(langNew);
									hmColumnIndexToLang.put(new Integer(nCurrentColumn), langNew);
								}
							}
						}
						// All columns headers must be found!
						boolean bIsFullyInitialized = true;
						for (int i=0; i<nColumnNumbers.length && bIsFullyInitialized; i++){
							bIsFullyInitialized = nColumnNumbers[i] >= 0;
						}
						if (!bIsFullyInitialized && !bIsPatch){
							parent.printError("The CSV does not contain all the expected columnn headers!");
							return false;
						} else {
							bIsInitialized = true;
							if (bIsPatch) { // Perform check whether languages in the patchfile exist in the dictionary
								Enumeration<Language> eImportLangs = vImportLanguages.elements();
								while (eImportLangs.hasMoreElements()) {
									Language l = eImportLangs.nextElement();
									if (!this.vLanguages.contains(l)) {
										parent.printWarning("The patchfile contains language '"+l+"', which was not found in the dictionary!");
									}
								}
							}
						}
						if (bIsFullyInitialized && !bIsPatch){
							vItems.add(new DictionaryComment(nLineNo,saCurrentRow));
							this.saLayout = saCurrentRow;
						}
						// Check whether language specific default languages are defined
						Language[] aLangs = this.getLanguages();
						for (int i=0; i<aLangs.length; i++){
							// Check LangMan properties first
							String sTempDefaultGlobal = parent.getSettingProperty(KEY_DEFAULT_LANG+"."+aLangs[i].getShortName(), false);
							String sTempDefaultLocal = this.getSettingProperty(KEY_DEFAULT_LANG+"."+aLangs[i].getShortName(), false);
							// If nothing found check whether the dictionary has own properties
							String sTempDefault = sTempDefaultGlobal;
							if (sTempDefaultGlobal == null){
								sTempDefault = sTempDefaultLocal;
							}
							if (sTempDefaultGlobal != null && sTempDefaultLocal != null){
								parent.printWarning( "The property '"+KEY_DEFAULT_LANG+"."+aLangs[i].getShortName()+"' of the dictionary was overwritten by your LangMan.properties");
							}
							if (sTempDefault != null){
								if (sTempDefault.equals(aLangs[i].getShortName())){
									parent.printWarning( "The default language for "+aLangs[i]+" cannot be "+sTempDefault+", because a language cannot be the default language for itself - default language will not be set!");
								}
								for (int j=0; j<aLangs.length; j++){
									if (aLangs[j].getShortName().equalsIgnoreCase(sTempDefault)){
										aLangs[i].setDefaultLang(aLangs[j]);
										break;
									}
								}
								if (aLangs[i].getDefaultLang() == null) {
									parent.printWarning( "The default language for '"+aLangs[i]+"' could not be set to '"+sTempDefault+"', because '"+sTempDefault+"' is unknown!");
								}
							}
						}
					} else {							
						String	sPart				= null;						
						String	sKey				= null;						
						DictionaryEntry reference 	= null;
						String 	sReference			= null;
						String	sComment			= null;						
						boolean	bChanged			= false;
						int 	nLimit				= -1;						
						
						HashMap<Language, String> hmCurrentVals = new HashMap<Language, String>();							 							
						
						for (int nCurrentColumn=0; nCurrentColumn<saCurrentRow.length; nCurrentColumn++){
							// switch not possible, because there might be "constants" twice in nColumnNumbers
							if (nCurrentColumn == nColumnNumbers[CLM_PART]){
								if (saCurrentRow[nCurrentColumn] != null && !saCurrentRow[nCurrentColumn].trim().equals("")){
									sPart = saCurrentRow[nCurrentColumn];
									if(!vParts.contains(sPart)){
										vParts.add(sPart);
									}
								} else {
									if(!bHideImportWarnings){
										parent.printWarning("Line "+nLineNo+" does not contain a valid part: "+saCurrentRow[nCurrentColumn]);
									}
								}									
								
							} else if (nCurrentColumn == nColumnNumbers[CLM_CHANGED]){
								bChanged = saCurrentRow[nCurrentColumn] != null && !saCurrentRow[nCurrentColumn].trim().equals("");
								
							} else if (nCurrentColumn == nColumnNumbers[CLM_KEY]){
								if (saCurrentRow[nCurrentColumn] != null && !saCurrentRow[nCurrentColumn].trim().equals("")){
									sKey = saCurrentRow[nCurrentColumn];
								} else {
									if(!bHideImportWarnings){
										parent.printWarning( "Line "+nLineNo+" does not contain a valid key: '"+saCurrentRow[nCurrentColumn]+"'");
									}
								}
							} else if (nCurrentColumn == nColumnNumbers[CLM_REFERENCE]){
								if (saCurrentRow[nCurrentColumn] != null && !saCurrentRow[nCurrentColumn].trim().equals("")){
									try {
										sReference = saCurrentRow[nCurrentColumn];
										String[] saReference = sReference.split("::", 2);
										reference = new DictionaryEntry(saReference[0], saReference[1]);
									} catch (Exception e) {
										if(!bHideImportWarnings){
											parent.printWarning( "Line "+nLineNo+" does not contain a valid reference: '"+saCurrentRow[nCurrentColumn]+"' which cause this error: "+e);
										}
									}
								}
								
							} else if (nCurrentColumn == nColumnNumbers[CLM_COMMENT]){
								if (saCurrentRow[nCurrentColumn] != null && !saCurrentRow[nCurrentColumn].trim().equals("")){
									sComment = saCurrentRow[nCurrentColumn];
								}
								
							} else if (nCurrentColumn == nColumnNumbers[CLM_LIMIT]){
								try {
									if(saCurrentRow[nCurrentColumn].trim().length() > 0){
										nLimit = Integer.valueOf(saCurrentRow[nCurrentColumn]).intValue();
									}
								} catch (Exception e){
									if(!bHideImportWarnings){
										parent.printWarning( "Line "+nLineNo+" does not contain a valid limit: '"+saCurrentRow[nCurrentColumn]+"'" );
									}
								}
								
							} else { // The language columns																			
								if (hmColumnIndexToLang.containsKey(new Integer(nCurrentColumn))){ // If this is a language column
									String sVal = null;										
									if (saCurrentRow[nCurrentColumn] != null && !saCurrentRow[nCurrentColumn].trim().equals("")){
										if (sEmptyString != null && saCurrentRow[nCurrentColumn].equals(sEmptyString)){
											sVal = "";
										} else {
											sVal = saCurrentRow[nCurrentColumn].replaceAll("\"\"", "\"");
										}
									}
									hmCurrentVals.put(hmColumnIndexToLang.get(new Integer(nCurrentColumn)), sVal);
								}
							}
						}
												
						// Add items to languages
						DictionaryEntry newEntry = new DictionaryEntry(
								sKey,
								hmCurrentVals,
								sComment,
								sPart,
								nLimit,
								bChanged,
								nLineNo,
								this,
								reference,
								sReference
						);
						
						if (reference != null) {
							vEntriesWithReferences.add(newEntry);
						}
						
						if(hmCurrentVals.size() < vImportLanguages.size() && !bHideImportWarnings){
							parent.printLog( "The key '"+sKey+"' on line "+nLineNo+" does not have a translation for every language!");
						}
						if (this.contains(newEntry)){
							if (bIsPatch){
								DictionaryEntry existingEntry = this.getEntry(newEntry);
								Language[] langs = vImportLanguages.toArray(new Language[vImportLanguages.size()]);							
								for (int i=0; i<langs.length; i++){									
									existingEntry.setValue(langs[i], hmCurrentVals.get(langs[i]));
								}
							} else {							
								parent.printError( "The dictionary already contains key '"+sKey+"' for part '"+sPart+"'! It is not added more than once and not overwritten!" );
								DictionaryComment comment = new DictionaryComment(nLineNo,saCurrentRow);
								comment.setIsDuplicateKey(true);
								vItems.add(comment);
							}
						} else {
							this.vItems.add(newEntry);
						}
					}
				} else if (!bIsPatch){
					vItems.add(new DictionaryComment(nLineNo,saCurrentRow));
				}
			}
		}
		
		// Update referencing entries from "part and key only" references to the real references
		Enumeration<DictionaryEntry>  enumReferencingEntries = vEntriesWithReferences.elements();
		while (enumReferencingEntries.hasMoreElements()) {
			DictionaryEntry currentReferencingEntry = enumReferencingEntries.nextElement();
			DictionaryEntry reference = this.getEntry(currentReferencingEntry.getReference());
			if (reference == null) {
				if(!bHideImportWarnings){
					parent.printWarning( "Line "+currentReferencingEntry.getLineNumber()+" does not contain a valid reference: '"+currentReferencingEntry.getReference()+"'" );
				}
				currentReferencingEntry.setReference(new BrokenReference(currentReferencingEntry.getReference().getPart(), currentReferencingEntry.getReference().getPart()));
			} else {
				currentReferencingEntry.setReference(reference);
			}
		}
		return bOk;		
	}
	
	
	
	public DictionaryEntry[] getPart(String sPart){
		Vector<DictionaryEntry> vEntries = new Vector<DictionaryEntry>();
		Enumeration<DictionaryItem> enumItems = this.vItems.elements();
		while(enumItems.hasMoreElements()){
			DictionaryItem currentItem = enumItems.nextElement();
			if (currentItem.isDictionaryEntry()){
				DictionaryEntry entry = (DictionaryEntry)currentItem;
				if (entry.getPart() != null && entry.getPart().equals(sPart)){
					vEntries.add((DictionaryEntry)currentItem);
				}				
			}			
		}
		return vEntries.toArray(new DictionaryEntry[vEntries.size()]);
	}
	
	
	public DictionaryEntry getEntry(String sPart, String sKey){
		DictionaryEntry lookupEntry = new DictionaryEntry(sPart, sKey);
		return this.getEntry(lookupEntry);
	}
	
	
	
	private DictionaryEntry getEntry(DictionaryEntry entry){
		if (vItems.contains(entry)){
			return (DictionaryEntry)vItems.elementAt(vItems.indexOf(entry));
		} else {
			return null;
		}
	}

	
	
	
	public DictionaryEntry[] getEntries(){
		Vector<DictionaryEntry> vEntries = new Vector<DictionaryEntry>();
		Enumeration<DictionaryItem> enumItems = this.vItems.elements();
		while(enumItems.hasMoreElements()){
			DictionaryItem currentItem = enumItems.nextElement();
			if (currentItem.isDictionaryEntry()){
				vEntries.add((DictionaryEntry)currentItem);
			}			
		}
		return vEntries.toArray(new DictionaryEntry[vEntries.size()]);
	}
	
	
	
	
}
