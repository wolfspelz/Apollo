package com.zweitgeist.tools.langman;

import java.awt.Point;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.util.Enumeration;
import java.util.Properties;
import java.util.Vector;

import javax.swing.DefaultListModel;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JOptionPane;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;
import javax.swing.table.DefaultTableModel;

import com.zweitgeist.tools.langman.data.DefaultDictionary;
import com.zweitgeist.tools.langman.data.Dictionary;
import com.zweitgeist.tools.langman.data.DictionaryEntry;
import com.zweitgeist.tools.langman.data.Language;

/**
 * LangMan is the main class of the LangMan. It holds the gui and the dictionary and handles all events
 * from the gui. 
 * @author jkuehne
 *
 */

public class LangMan implements Constants {
	
	/** The gui of this LangMan */
	private LangManForm 	form;
	/** The settings for this LangMan found in SETTINGS_FILE */
	private Properties		prpSettings;
	/** The filelogger of this LangMan */
	private BufferedWriter	bwLog;
	/** Flag indicating whether this LangMan runs in GuiMode (true) or as a command line tool (false) */
	private boolean			bIsInGuiMode;
	/** Flag indicating whether warnings on import should be omitted */
	private boolean			bHideImportWarnings;
	/** The Dictionary of this LangMan - it contains all the information about the languages/translations */
	private Dictionary		dictionary;
	/** Log level that indicates what output is written to the console */
	private int				nLogLevelConsole;
	/** Log level that indicates what output is written to the log file */
	private int				nLogLevelFile;
	
	
	public LangMan(boolean bHideImportWarnings, String sConfigFile){
		this.bHideImportWarnings = bHideImportWarnings;
		//this.sbLog = new StringBuffer();
		this.prpSettings = new Properties();
		
		try{
			prpSettings.load(new FileInputStream(new File((sConfigFile==null)?SETTINGS_FILE_GLOBAL:sConfigFile)));
			File fLocal = new File(SETTINGS_FILE_LOCAL);
			if (fLocal.exists()){
				prpSettings.load(new FileInputStream(fLocal));
			}
		} catch ( Exception e ){
			printError( "Could not load settings - exiting!: "+e );
			exitApp(1);
		}
		
		// Set LogLevels
		this.nLogLevelConsole = LOG_LEVEL_LOG;
		this.nLogLevelFile = LOG_LEVEL_LOG;
		String sLogLevel = getSettingProperty(KEY_LOG_LEVEL, false);
		if (sLogLevel != null && !sLogLevel.equals("")){
			int nTempLevel = getLogLevel(sLogLevel);
			if (nTempLevel > -1){
				this.nLogLevelConsole = nTempLevel;
				this.nLogLevelFile = nTempLevel;
			}
		}
		sLogLevel = getSettingProperty(KEY_LOG_LEVEL+"."+KEY_LOG_CHANNEL_CONSOLE, false);
		if (sLogLevel != null && !sLogLevel.equals("")){
			int nTempLevel = getLogLevel(sLogLevel);
			if (nTempLevel > -1){
				this.nLogLevelConsole = nTempLevel;
			}
		}
		sLogLevel = getSettingProperty(KEY_LOG_LEVEL+"."+KEY_LOG_CHANNEL_FILE, false);
		if (sLogLevel != null && !sLogLevel.equals("")){
			int nTempLevel = getLogLevel(sLogLevel);
			if (nTempLevel > -1){
				this.nLogLevelFile = nTempLevel;
			}
		}
		
		try {
			this.bwLog = new BufferedWriter(new FileWriter(new File(getSettingProperty(KEY_LOG_FILE))));
		} catch (Exception e){
			printError( "Could not open logfile!: "+e );
		}
		String sDoUnitTests = getSettingProperty(KEY_RUN_UNIT_TESTS, false);
		if (sDoUnitTests != null && sDoUnitTests.trim().equalsIgnoreCase("true")){
			if (!UnitTest.testAll()){
				System.err.println( "UnitTests failed - check the log!");
				System.exit(1);
			}
		}
	}
	
	
	
	/**
	 * 
	 * @param sLogLevel
	 * @return int value representing the log level, -1 in case of error
	 */
	private int getLogLevel(String sLogLevel){
		boolean bFoundLevel = false;
		int result = -1;
		for (int i=0; i<LOG_LEVEL_NAMES.length; i++){
			if (sLogLevel.equalsIgnoreCase(LOG_LEVEL_NAMES[i])){
				result = i;
				bFoundLevel = true;
				break;
			}
		}
		if (!bFoundLevel){
			printError("The config file contains an unknown loglevel: '"+sLogLevel+"'!");
		}
		return result;
	}
	
	
	
	private void exitApp(int nErrorCode){
		try {
			this.bwLog.close();
		} catch (Exception e){
			printError("Could not close logfile!: "+e);
		}
		System.exit(nErrorCode);
	}
	
	
	
	private boolean initDictionary(){
		String sFileName = getSettingProperty(KEY_CSV_FILE_NAME);
		File fCsv = null;
		File fConfig = null;
		boolean bOk = true;
		if (sFileName == null) {
			printError("Could not retrieve csv file from config!");
			bOk = false;			
		}
		if (bOk) {
			fCsv = new File(sFileName);
			if (fCsv == null || !fCsv.exists()) {
				printError("Could not load csv file defined in config!");
				bOk = false;
			}
		}
		
		if (bOk) {
			fConfig = Tools.replaceExtension(fCsv, "csv", PROPERTIES_EXTENSION);
			if (fConfig == null || !fConfig.exists()) {
				printError("Could not load config file for csv defined in config!");
				bOk = false;
			}
		}
		
		if (bOk) {
			this.dictionary = new DefaultDictionary(
				fCsv,
				fConfig,
				getSettingProperty(KEY_DEFAULT_LANG),
				getSettingProperty(KEY_EMPTY_STRING),
				this, 
				this.bHideImportWarnings);
		}
		return this.dictionary != null;
	}
	
	
	
	public void printHelp(){
		System.out.println(
				"Usage: LanguageManager [-?|-help] [-gui] [-hideImportWarnings] [-l|-lang LANGUAGE]* [-p|-part PART]*"+
				"Known arguments:\n" +
				"-?\tPrints this help\n" +
				"-help\tPrints this help\n" +
				"-gui\tStarts in gui mode (default=false)\n" +
				"-l LANGUAGE\tExport language LANGUAGE\n" +
				"-lang LANGUAGE\tExports language LANGUAGE\n" +
				"-p PART\tExports part PART\n" +
				"-part PART\tExports part PART\n" +
				"-hideImportWarnings\tWarnings of the import are not shown!" +
				"-v\tPrints the version of the LangMan you are using\n" +
				"-version\tPrints the version of the LangMan you are using\n" +
				"-c CONFIG_FILE_NAME\tThe file containing the configuration for the LangMan - defaults to 'LangMan.properties'\n" +
				"-config CONFIG_FILE_NAME\tThe file containing the configuration for the LangMan - defaults to 'LangMan.properties'\n" +
				"If part and language are give only part PART of language LANGUAGE ist exported. By default " +
				"all parts of all languages are exported. If started in gui mode all export arguments are omitted!"
		);
	}
	
	
	
	private void logToConsole(String sLogEntry, int nEntryLevel){
		if (nEntryLevel >= this.nLogLevelConsole){
			if (nEntryLevel == LOG_LEVEL_ERROR){
				System.err.println(sLogEntry);
			} else {
				System.out.println(sLogEntry);
			}
		}
	}
	
	
	
	private void logToFile(String sLogEntry, int nEntryLevel){
		if (bwLog != null && nEntryLevel >= this.nLogLevelFile){
			try {
				sLogEntry = sLogEntry.replaceAll("\\n", "\t");
				bwLog.append(sLogEntry+"\n");
				bwLog.flush();
			} catch (Exception e){
				System.err.println("Could not write to logfile: "+e);
			}
		}
	}
		
	
	
	public void printError(Object o){
		String sMessage = "ERROR: "+o;
		logToConsole(sMessage, LOG_LEVEL_ERROR);
		logToFile(sMessage, LOG_LEVEL_ERROR);		
		if (bIsInGuiMode){
			JOptionPane.showMessageDialog(this.form.getFrmMain(), o.toString(), "Error", JOptionPane.ERROR_MESSAGE);
		}
	}
	
	
	
	public void printWarning(Object o){
		String sMessage = "WARNING: "+o;
		logToConsole(sMessage, LOG_LEVEL_WARN);
		logToFile(sMessage, LOG_LEVEL_WARN);
		
		if (bIsInGuiMode){
			JOptionPane.showMessageDialog(this.form.getFrmMain(), sMessage, "Warning", JOptionPane.WARNING_MESSAGE);
		}
	}
	
	
	
	public void printLog(Object o){
		String sMessage = "LOG: "+o;
		logToConsole(sMessage, LOG_LEVEL_LOG);
		logToFile(sMessage, LOG_LEVEL_LOG);
	}
	
	
	
	public void printMessage(Object o){
		String sMessage = o.toString();
		logToConsole(sMessage, LOG_LEVEL_MSG);
		logToFile(sMessage, LOG_LEVEL_MSG);
		if (bIsInGuiMode){
			JOptionPane.showMessageDialog(this.form.getFrmMain(), o.toString(), "Info", JOptionPane.INFORMATION_MESSAGE);
		}
	}
	
	

	/**
	 * @param args
	 */
	public static void main(String[] args) {		
		boolean bStartGui = false;
		boolean bPrintHelp = false;
		boolean bPrintVersion = false;
		boolean bHideImportWarnings = false;
		String  sConfigFile = null;
		Vector<Language> vArgLangs = new Vector<Language>();
		Vector<String> vArgParts = new Vector<String>();
		
		for (int i=0; i<args.length; i++){
			if ( args[i].equalsIgnoreCase("-help") || args[i].equals("-?")){
				bPrintHelp = true;
			} else if (args[i].equalsIgnoreCase("-gui")){
				bStartGui = true;
			} else if (args[i].equalsIgnoreCase("-l") || args[i].equalsIgnoreCase("-lang")){
				vArgLangs.add(new Language(args[++i]));
			} else if (args[i].equalsIgnoreCase("-p") || args[i].equalsIgnoreCase("-part")){
				vArgParts.add(args[++i]);
			} else if (args[i].equalsIgnoreCase("-hideImportWarnings")){
				bHideImportWarnings = true;
			} else if (args[i].equalsIgnoreCase("-v") || args[i].equalsIgnoreCase("-version")){
				bPrintVersion = true;
			} else if (args[i].equalsIgnoreCase("-c") || args[i].equalsIgnoreCase("-config")){
				sConfigFile = args[++i]; 
			} else {
				bPrintHelp = true;
			}
		}
		if (bPrintVersion){
			System.out.println("The version of this LangMan is "+VERSION);
		}
		final boolean bHideImportWarningsFinal = bHideImportWarnings;
		final String  sConfigFileFinal = sConfigFile;
		
		if (bPrintHelp){
			LangMan langMan = new LangMan(bHideImportWarnings, sConfigFile);
			langMan.printHelp();
		} else if (bStartGui){
			try {
			    // Set System Look & Feel
		        UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		    } catch (Exception e) {
		        System.err.println( "System 'Look & Feel' could not be set!" );
		    }
	
			SwingUtilities.invokeLater(new Runnable() {
				LangMan langMan = new LangMan(bHideImportWarningsFinal, sConfigFileFinal);
				public void run() {
					LangManForm form = new LangManForm(langMan);
					form.getFrmMain().setTitle( "LangMan v"+VERSION);
					langMan.setForm(form);
					form.getFrmMain().setVisible(true);
					if (!langMan.initDictionary()){
						langMan.printError("Could not init dictionary - check the log!");
					} else {
						langMan.updateTblMain();
					}
				}
			});
		} else {
			LangMan langMan = new LangMan(bHideImportWarnings, sConfigFile);			
			if (!langMan.initDictionary()){
				langMan.printError("Could not init dictionary - check the log!");
			}
			if(vArgLangs.size() == 0 && vArgParts.size() == 0){
				if(langMan.export()){
					langMan.printMessage("Export worked fine :-)!");
				} else {
					langMan.printError("Export failed - please check the log!");
				}
								
			} else {
				Vector<Language> vLangs = new Vector<Language>(vArgLangs.size());
				Enumeration<Language> e = vArgLangs.elements();
				while (e.hasMoreElements()) {
					String sLang = e.nextElement().toString();
					Language tempLang = langMan.dictionary.getLanguage(sLang);
					if (tempLang == null) {
						langMan.printWarning("I am supposed to export language '"+sLang+"', but it is not part of the dictionary and so I am not able to fulfill your wish!");
					} else {
						vLangs.add(tempLang);
					}
				}
				if(langMan.export(vLangs.toArray(new Language[vLangs.size()]), vArgParts.toArray(new String[vArgParts.size()]))){
					langMan.printMessage("Export worked fine :-)!");
				} else {
					langMan.printError("Export failed - please check the log!");					
				}
			}
		}
	}
	
	
	
	public void setForm( LangManForm form ){
		this.form = form;
		this.bIsInGuiMode = true;
	}
	
	
	
	private void addAdditionalTargets(String sPart, Vector<String> vExportTargets) {
		String sKey = KEY_ADDITIONAL_TARGET+"."+sPart+".";
		int i = 1;
		String sValue = getSettingProperty(sKey+i, false);
		while (sValue != null) {
			vExportTargets.add(sValue);
			sValue = getSettingProperty(sKey+(++i), false);
		}				
	}
	

	
	public boolean export(Language lang, String sPart) {
		boolean bOk = true;
		Vector<String> vExportTargets = new Vector<String>();
		vExportTargets.add(sPart);
		addAdditionalTargets(sPart, vExportTargets);
		Enumeration<String> enumParts = vExportTargets.elements();
		while (enumParts.hasMoreElements()) {
			String sTarget = enumParts.nextElement();			
		
			String sTemplatePath = getSettingProperty(KEY_TEMPLATE_PATH);
			String sFileTemplateName = getSettingProperty(KEY_FILE_TEMPLATE+"."+sTarget);		
			String sLineTemplateName = getSettingProperty(KEY_LINE_TEMPLATE+"."+sTarget, false);		
			String sExportDir = getSettingProperty(KEY_EXPORT_PATH+"."+sTarget+"."+lang.getShortName(), false);
			if (sExportDir == null){
				sExportDir = getSettingProperty(KEY_EXPORT_PATH+"."+sTarget);
			}
			
			String sExportFile = getSettingProperty(KEY_EXPORT_NAME+"."+sTarget+"."+lang.getShortName(), false);
			if (sExportFile == null) {
				sExportFile = getSettingProperty(KEY_EXPORT_NAME+"."+sTarget);
			}
			sExportFile = sExportDir+sExportFile;
			String sBoundaryCharVal = getSettingProperty(KEY_BOUNDARY_CHAR_VAL+"."+sTarget);
			String sEscapeCharVal = getSettingProperty(KEY_ESCAPE_CHAR_VAL+"."+sTarget);
			String sBoundaryCharKey = getSettingProperty(KEY_BOUNDARY_CHAR_KEY+"."+sTarget, false);
			String sEscapeCharKey = getSettingProperty(KEY_ESCAPE_CHAR_KEY+"."+sTarget, false);
			String sKeyLang = getSettingProperty(KEY_LANG);
			
			if (sKeyLang != null && lang != null){
				if (sFileTemplateName != null){
					sFileTemplateName = sTemplatePath+sFileTemplateName.replaceAll(sKeyLang, lang.getShortName());
				} else {
					printError("LangMan.export( "+lang+", "+sPart+"): sFileTemplate for target "+sTarget+" is null!");
					bOk = false;
					continue;
				}
				if (sLineTemplateName != null){
					sLineTemplateName = sTemplatePath+sLineTemplateName.replaceAll(sKeyLang, lang.getShortName());
				}// No else -> lineTeplate might be null
				if (sExportFile != null){
					sExportFile = sExportFile.replaceAll(sKeyLang, lang.getShortName());
				} else {
					printError("LangMan.export( "+lang+", "+sPart+"): sExportFile for target "+sTarget+" is null!");
					bOk = false;
					continue;
				}
				
			} else {
				printError("LangMan.export( "+lang+", "+sPart+"): sKeyLang or lang for target "+sTarget+" is null!");
				bOk = false;
				continue;				
			}		
			String sContentToExport = null;
			File fLineTemplate = null;
			if (sLineTemplateName != null){
				fLineTemplate = new File(sLineTemplateName);
			}
			if (lang != null && sFileTemplateName != null && sBoundaryCharVal != null && sEscapeCharVal != null ){
				sContentToExport = fillTemplate(
						lang,
						sTarget,
						dictionary.getPart(sPart), 
						new File(sFileTemplateName), 
						fLineTemplate,
						sBoundaryCharVal,
						sEscapeCharVal,
						sBoundaryCharKey,
						sEscapeCharKey
				);
			}
			
			File fExportFile;	
			if (sExportFile != null && sContentToExport != null){
				fExportFile = new File(sExportFile);
				try{			
					if (!fExportFile.exists()){
						sExportDir = sExportDir.replaceAll(sKeyLang, lang.getShortName());
						File fExportDir = new File(sExportDir);
						if (!fExportDir.exists()){
							if (!fExportDir.mkdirs()){
								printError("Could not create export directory: "+fExportFile.getPath()+"!");
							}
						}
						if (!fExportFile.createNewFile()){
							printError("Could not create export file: "+fExportFile+"!");
						}
						
					}
					String sCharset = getSettingProperty(KEY_ENCODING+"."+sTarget+"."+lang.getShortName().toLowerCase(), false);
					if (sCharset == null){ // If no special encoding defined for part.language check encoding for part 
						sCharset = getSettingProperty(KEY_ENCODING+"."+sTarget, false);
					}
					Tools.writeTextFile(fExportFile, sContentToExport, sCharset);
				} catch (Exception e){
					printError("Could not export file '"+fExportFile+"': "+e);
					bOk = false;
					continue;
				}
			} else {
				bOk = false;
				continue;
			}
			if (fExportFile != null){
				printLog("Generated file '"+fExportFile+"'");
			}
		}
		return bOk;
	}
	
	
	
	public boolean export(Language lang){
		boolean result = true;		
		String[] saParts = this.dictionary.getParts();
		for (int i=0; i<saParts.length; i++){
			boolean bExportOk = export(lang, saParts[i]);
			if(!bExportOk){
				printError("Could not export part '"+saParts[i]+"' of language '"+lang+"' - check the log!");
			}
			result &= bExportOk;
		}
		return result;
	}
	
	
	
	public boolean export(String sPart){
		boolean result = true;
		Language[] langs = this.dictionary.getLanguages();
		for (int i=0; i<langs.length; i++){
			boolean bExportOk = export(langs[i], sPart);
			if(!bExportOk){
				printError("Could not export part '"+sPart+"' of language '"+langs[i]+"' - check the log!");
			}
			result &= bExportOk;
		}
		return result;
	}
	
	
	
	public boolean export(){
		return export(this.dictionary.getLanguages(), this.dictionary.getParts());
	}
	
	
	
	public boolean export(Language[] langs, String[] parts){
		boolean bResult = true;
		Language[] tempLangs;		
		if (langs == null || langs.length==0){
			tempLangs = this.dictionary.getLanguages();
		} else {
			tempLangs = langs;
		}		
		for (int i=0; i<tempLangs.length; i++){
			if (!this.dictionary.contains(tempLangs[i])){
				printWarning("Cannot export language '"+tempLangs[i]+"' because it is not part of the csv I loaded!");
				continue;				
			}
			String[] tempParts;
			if (parts == null || parts.length == 0){
				tempParts = this.dictionary.getParts();
			} else {
				tempParts = parts;
			}
			for (int nPart = 0; nPart<tempParts.length; nPart++){				
				if (!this.dictionary.containsPart(tempParts[nPart])){
					printWarning("Cannot export part '"+tempParts[nPart]+"' because it is not part of the csv I loaded!");
					continue;
				}
				boolean bExportOk = export(tempLangs[i], tempParts[nPart]);				
				if(!bExportOk){
					printError("Could not export part '"+tempParts[nPart]+"' of language '"+tempLangs[i]+"' - check the log!");
				}
				bResult &= bExportOk;
			}
		}
		return bResult;
	}
	
	
	
	public void export(Language[] langs){
		for (int i=0; i<langs.length; i++){
			if (!this.dictionary.contains(langs[i])){				
				printWarning("Cannot export language '"+langs[i]+"' because it is not part of the csv I loaded!");
				continue;
			}				
			if (!export(langs[i])) {
				printError("Could not export language '"+langs[i]+"' - check the log!");
			}
		}
	}
	

	
	public String getSettingProperty(String sKey, boolean bVerbose){
		String sVal = this.prpSettings.getProperty(sKey);
		if (sVal == null && bVerbose){
			printError("Could not find a val for key '"+sKey+"' in the config file!");
		}
		return sVal;
	}
	
	
	
	private String getSettingProperty(String sKey){
		return getSettingProperty(sKey, true);		
	}
	
	
	
	private String fillTemplate(
			Language lang,
			String sPart,
			DictionaryEntry[] entries,
			File fFileTemplate,
			File fLineTemplate,
			String sBoundaryCharVal,
			String sEscapeCharVal,
			String sBoundaryCharKey,
			String sEscapeCharKey) {
		String sFileTemplate = null;
		String sLineTemplate = null;
		String result = "";
		dictionary.resetStatistics();
		try{
			String sEncoding = getSettingProperty(KEY_FILE_TEMPLATE+"."+KEY_ENCODING+"."+sPart, false);
			if (sEncoding == null){
				sEncoding = getSettingProperty(KEY_FILE_TEMPLATE+"."+KEY_ENCODING, false);
			}
			sFileTemplate = Tools.readTextFile(fFileTemplate, sEncoding);
		} catch (Exception e){
			printError("Could not read file for filetemplate "+fFileTemplate+": "+e);
			return null;
		}
//		NoValBehavior behavior = NoValBehavior.EMPTY;
		int nBehavior = NVB_EMPTY;
		String sBehavior = getSettingProperty(KEY_NO_VAL_FOUND+"."+sPart, false);
		if (sBehavior == null){
			sBehavior = getSettingProperty(KEY_NO_VAL_FOUND, false);
		}
		if (sBehavior != null){
			boolean bFoundValidBehavior = false;
			for (int i=0; i<S_BEHAVIORS.length; i++){
				if (sBehavior.equalsIgnoreCase(S_BEHAVIORS[i])){
					nBehavior = i;
					bFoundValidBehavior = true;
					break;
				}
			}
			if (!bFoundValidBehavior){
				printWarning("The value ('"+sBehavior+"') set for key 'NoValFound[.PART]' in your properties is not valid - using the default (empty)!");
			}
		}
		if (fLineTemplate != null){
			try{
				String sEncoding = getSettingProperty(KEY_LINE_TEMPLATE+"."+KEY_ENCODING+"."+sPart, false);
				if (sEncoding == null){
					sEncoding = getSettingProperty(KEY_LINE_TEMPLATE+"."+KEY_ENCODING, false);
				}
				sLineTemplate = Tools.readTextFile(fLineTemplate, sEncoding);
			} catch (Exception e){
				printError("Could not read file for linetemplate "+fLineTemplate+": "+e);
				return null;
			}
		}
		StringBuffer sbItems = new StringBuffer();
//		String sBoundaryCharDoubleBackslashed = sBoundaryChar.replaceAll("\\\\", "\\\\\\\\");
//		sEscapeChar = sEscapeChar.replaceAll("\\\\", "\\\\\\\\");
		String sUseDefaultLang = getSettingProperty(KEY_USE_DEFAULT_LANG+"."+sPart, false);
		boolean bUseDefaultLang = !(sUseDefaultLang != null && sUseDefaultLang.equalsIgnoreCase("false"));
		for (int i=0; i<entries.length; i++){
			String sNewLine = sLineTemplate;
			String sKey = entries[i].getKey();
			if (sKey != null){
				if (sBoundaryCharKey != null && sEscapeCharKey != null) {
					sKey = Tools.escapeString(sKey, sBoundaryCharKey, sEscapeCharKey);
				}
				sKey = sKey.replaceAll("\\\\", "\\\\\\\\");			
				sKey = sKey.replaceAll("\\$", "\\\\\\$");
				if (sBoundaryCharKey != null) {
					sKey = sBoundaryCharKey+sKey+sBoundaryCharKey;
				}
				String sVal = entries[i].getValue(lang, bUseDefaultLang);
				if (sVal != null){
					sVal = Tools.escapeString(sVal, sBoundaryCharVal,sEscapeCharVal);
//					sVal = sVal.replaceAll(sBoundaryCharDoubleBackslashed, sEscapeChar+sBoundaryCharDoubleBackslashed);
					sVal = sVal.replaceAll("\\\\", "\\\\\\\\");			
					sVal = sVal.replaceAll("\\$", "\\\\\\$");
					sVal = sBoundaryCharVal+sVal+sBoundaryCharVal;
				}
				
				if (sLineTemplate == null){ // If no line template is given it is assumed that all keys should be replaced in the file template
					if (sVal == null){
						if (nBehavior == NVB_EMPTY){ // NVB_OMMIT not applicable for FileTemplateOnly; NVB_KEY -> do nothing and keys stay in
							sFileTemplate = sFileTemplate.replaceAll(sKey, "");
						}						
					} else {
						sFileTemplate = sFileTemplate.replaceAll(sKey, sVal);
					}
					
				} else {
					if (sVal == null){
						switch (nBehavior){
							case NVB_EMPTY:
								sNewLine = sNewLine.replaceAll(getSettingProperty(KEY_KEY), sKey);
								sNewLine = sNewLine.replaceAll(getSettingProperty(KEY_VAL), sBoundaryCharVal+sBoundaryCharVal);
								sbItems.append(sNewLine);
								break;
							case NVB_OMMIT: 
								break;
							case NVB_KEY:
								sNewLine = sNewLine.replaceAll(getSettingProperty(KEY_KEY), sKey);
								sNewLine = sNewLine.replaceAll(getSettingProperty(KEY_VAL), sBoundaryCharVal+sKey+sBoundaryCharVal);
								sbItems.append(sNewLine);
								break;
						}
					} else {
						sNewLine = sNewLine.replaceAll(getSettingProperty(KEY_KEY), sKey);
						sNewLine = sNewLine.replaceAll(getSettingProperty(KEY_VAL), sVal);
						sbItems.append(sNewLine);
					}
				}				
			}			
		}
		result = sFileTemplate;
		result = result.replaceAll(getSettingProperty(KEY_LANG), lang.getShortName());
		String sItems = sbItems.toString();
		sItems = sItems.replaceAll("\\\\", "\\\\\\\\");
		sItems = sItems.replaceAll("\\$", "\\\\\\$");
		result = result.replaceAll(getSettingProperty(KEY_ITEMS), sItems);
		
		printStatistics(lang, sPart);
		
		return result;
	}
	
	
	
	public void printStatistics(Language lang, String sPart){
		DictionaryEntry[] aKeysWithoutVal = dictionary.getStatistics(Dictionary.ST_KEYS_WITHOUT_VALS);		
		StringBuffer sbKeysWithoutVal = new StringBuffer();
		if (aKeysWithoutVal != null && aKeysWithoutVal.length > 0){
			for (int i=0; i<aKeysWithoutVal.length; i++){
				sbKeysWithoutVal.append("- "+aKeysWithoutVal[i].getKey()+"\n");
			}
		}
		
		DictionaryEntry[] aDefaultReplacements = dictionary.getStatistics(Dictionary.ST_DEFAULT_REPLACEMENTS);
		StringBuffer sbDefaultReplacements = new StringBuffer();
		if (aKeysWithoutVal != null && aDefaultReplacements.length > 0){
			for (int i=0; i<aDefaultReplacements.length; i++){
				sbDefaultReplacements.append("- "+aDefaultReplacements[i].getKey()+"\n");
			}
		}
		if (aKeysWithoutVal.length > 0 || aDefaultReplacements.length > 0){
			printWarning("\nLanguage: "+lang+"   Part: "+sPart+"\nI found "+aKeysWithoutVal.length+" keys without values and I took "+aDefaultReplacements.length+" values from the default language. Check the log to see the keys.");
			if (aKeysWithoutVal.length > 0){
				printLog("These keys had no values:\n"+sbKeysWithoutVal);
			}
			if (aDefaultReplacements.length > 0){
				printLog("These keys were taken from the default language:\n"+sbDefaultReplacements);
			}
		}
	}
	
	
	
	protected void handleEvents(int nEvent){
		switch( nEvent ){
			case EVT_MI_LOAD_CSV_PRESSED:{
				JFileChooser fc = new JFileChooser();
				int returnVal = fc.showOpenDialog( form.getFrmMain() );
				if (returnVal == JFileChooser.APPROVE_OPTION) {
		            File fCsv = fc.getSelectedFile();		            
		            File fConfig = Tools.replaceExtension(fCsv, "csv", PROPERTIES_EXTENSION);
		            this.dictionary = new DefaultDictionary(
		            		fCsv,
		            		fConfig,
		            		getSettingProperty(KEY_DEFAULT_LANG), 
		            		getSettingProperty(KEY_EMPTY_STRING), 
		            		this, 
		            		this.bHideImportWarnings
		            );
		            if (this.dictionary != null){		            	
		            	updateTblMain();		            	
		            }		            
		        }
			}break;
				
			case EVT_MI_LOAD_PATCH_PRESSED:{
				JFileChooser fc = new JFileChooser();
				int returnVal = fc.showOpenDialog( form.getFrmMain() );
				if (returnVal == JFileChooser.APPROVE_OPTION) {
		            File file = fc.getSelectedFile();
		            if (this.dictionary.loadPatch(file, this.bHideImportWarnings)){
		            	updateTblMain();
		            	printMessage("Patched dictionary successfully!");
		            } else {
		            	printError("Could not patch dictionary - check the log!");
		            }
		        }
			} break;
				
			case EVT_MI_SAVE_CSV_PRESSED:{
				JFileChooser fc = new JFileChooser();
				int returnVal = fc.showSaveDialog(form.getFrmMain());
				if (returnVal == JFileChooser.APPROVE_OPTION) {
		            File file = fc.getSelectedFile();
		            if (file!=null){
		            	if (!file.exists() || 
		            			file.exists() && JOptionPane.YES_OPTION == JOptionPane.showConfirmDialog(this.form.getFrmMain(), "Are you sure that you want to overwrite '"+file+"'?")){
				            if (dictionary.exportToCsv(file)){
				            	printMessage("Wrote csv successfully!");			            	
				            } else {
				            	printError("Something went wrong writing the csv to "+file+". Please check the log!");
				            }
		            	}
		            }
		        }
			} break;
				
			case EVT_MI_GENERATE_PRESSED:{
				JDialog dlgExport = form.getDlgGenerate();				
				Point loc = form.getFrmMain().getLocation();
				loc.translate(20, 20);
				dlgExport.setLocation(loc);
				
				DefaultListModel modelLangs = new DefaultListModel();
				Language[] langs = this.dictionary.getLanguages();
				for (int i=0; i<langs.length; i++){
					modelLangs.addElement(langs[i]);
				}
				form.lstGenerateLangs.setModel(modelLangs);
				
				DefaultListModel modelParts = new DefaultListModel();
				String[] saParts = this.dictionary.getParts();
				for (int i=0; i<saParts.length; i++){
					modelParts.addElement(saParts[i]);
				}
				form.lstGenerateParts.setModel(modelParts);
				
				dlgExport.pack();
				dlgExport.setVisible(true);
			} break;
				
			case EVT_BTN_EXPORT_CANCEL_PRESSED:
				form.getDlgGenerate().dispose();
				break;
				
			case EVT_BTN_EXPORT_OK_PRESSED:{
				Object[] oLangs = form.lstGenerateLangs.getSelectedValues();				
				Language[] langs = new Language[oLangs.length];
				System.arraycopy(oLangs, 0, langs, 0, oLangs.length);
				Object[] oParts = form.lstGenerateParts.getSelectedValues();
				String[] parts = new String[oParts.length];
				System.arraycopy(oParts, 0, parts, 0, oParts.length);
				if(export(langs, parts)){
					printMessage("Export worked fine :-)!");
				} else {
					printError("Something went wrong with the export - please check the log!");
				}
				
				form.getDlgGenerate().dispose();
			} break;
				
			case EVT_MI_IMPORT_PRESSED:
				break;
				
			case EVT_MI_TRANSLATOR_PRESSED:{
				JDialog dlgTranslator = form.getDlgTranslator();				
				Point loc = form.getFrmMain().getLocation();
				loc.translate(20, 20);
				dlgTranslator.setLocation(loc);
				
				DefaultListModel modelLangs = new DefaultListModel();
				Language[] langs = this.dictionary.getLanguages();
				for (int i=0; i<langs.length; i++){
					modelLangs.addElement(langs[i]);
				}
				form.getLstTranslatorSources().setModel(modelLangs);
				form.getLstTranslatorTargets().setModel(modelLangs);
				
				DefaultListModel modelParts = new DefaultListModel();
				String[] saParts = this.dictionary.getParts();
				for (int i=0; i<saParts.length; i++){
					modelParts.addElement(saParts[i]);
				}
				form.getLstTranslatorParts().setModel(modelParts);
				dlgTranslator.pack();
				dlgTranslator.setVisible(true);
				}
				break;
				
			case EVT_BTN_TRANSLATOR_CHOOSE_PRESSED:{
				JFileChooser fc = new JFileChooser();
				//fc.setDialogType(JFileChooser.SAVE_DIALOG);
				fc.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
				int returnVal = fc.showOpenDialog( form.getDlgTranslator() );
				if (returnVal == JFileChooser.APPROVE_OPTION) {
		            File file = fc.getSelectedFile();
		            form.getTfTranslatorFile().setText( file.getAbsolutePath() );
		        }
			} break;
				
			case EVT_BTN_TRANSLATOR_CANCEL_PRESSED:
				form.getDlgTranslator().dispose();
				break;
				
			case EVT_BTN_TRANSLATOR_OK_PRESSED:{
				Object[] oSources = form.getLstTranslatorSources().getSelectedValues();				
				Language[] sources = new Language[oSources.length];
				System.arraycopy(oSources, 0, sources, 0, oSources.length);
				
				Object[] oTargets = form.getLstTranslatorTargets().getSelectedValues();				
				Language[] targets = new Language[oTargets.length];
				System.arraycopy(oTargets, 0, targets, 0, oTargets.length);
				
//				Language target = (Language)form.getLstTranslatorTarget().getSelectedValue(); 
				
				Object[] oParts = form.getLstTranslatorParts().getSelectedValues();
				String[] parts = new String[oParts.length];
				System.arraycopy(oParts, 0, parts, 0, oParts.length);
			
				String sDirName = form.getTfTranslatorFile().getText();
				if ( !(sDirName != null && sDirName.trim().length() > 0)) {
					printError("Please enter a directory!");
				} else if (sources.length == 0) {
					printError("Please select at least one source language!");
				} else if (targets.length < 1) {
					printError("Please select at least one target language!");
				} else if (parts.length < 1){
					printError("Please select at least one part!");
				} else {
					boolean bErrorOccured = false;
					for (int i=0;i <targets.length; i++){
						String sFileName = sDirName+"/"+getSettingProperty(KEY_TRANSLATOR_FILE);
						sFileName = sFileName.replaceAll(getSettingProperty(KEY_LANG), targets[i].getShortName());
						File file = new File(sFileName);
						if (!file.exists() || 
								file.exists() && JOptionPane.YES_OPTION == JOptionPane.showConfirmDialog(this.form.getFrmMain(), "Are you sure that you want to overwrite '"+file+"'?")){
							if (this.dictionary.generateTranslatorFile(
									new File (sFileName),
									targets[i],
									sources,
									parts)){
								printLog("Generated file '"+sFileName+"'");
								form.getDlgTranslator().dispose();
							} else {
								printError("Something went when I tried to generate the translator file for '"+targets[i]+"- please check the log!");
								bErrorOccured = true;
							}
						}
					}
					if (!bErrorOccured){
						printMessage("Everything worked fine :-)!"); 
					}					
				}
					
								
			} break;		
		}
	}
	
	
	
	private void updateTblMain(){
		Language[] langs = this.dictionary.getLanguages();
		int nPartCol		= 0;
		int nChangedCol		= 1;
		int nKeyCol			= 2;
		int nFirstLangCol	= 3;
		int nCommentCol		= nFirstLangCol+this.dictionary.getLanguages().length;
		DictionaryEntry[] entries = this.dictionary.getEntries();
		DefaultTableModel model = new DefaultTableModel(entries.length, 0);
		model.addColumn("Part");
		model.addColumn("Changed");
		model.addColumn("Key");
		for (int i=0; i<langs.length; i++){			
			Language lang = langs[i];
			model.addColumn(lang.getShortName());
		}		
		model.addColumn("Comment");		
		for (int j=0; j<entries.length; j++){
			String sUseDefaultLang = getSettingProperty(KEY_USE_DEFAULT_LANG+"."+entries[j].getPart(), false);
			boolean bUseDefaultLang = !(sUseDefaultLang != null && sUseDefaultLang.equalsIgnoreCase("false"));
			model.setValueAt(entries[j].getKey(), j, nKeyCol);
			model.setValueAt(entries[j].getHasChanged(), j, nChangedCol);
			model.setValueAt(entries[j].getPart(), j, nPartCol);
			model.setValueAt(entries[j].getComment(), j, nCommentCol);
			for (int i=0; i<langs.length; i++){
				model.setValueAt(entries[j].getValue(langs[i], bUseDefaultLang), j, i+nFirstLangCol);				
			}
		}
		form.tblMain.setModel(model);		
		form.miLoadPatch.setEnabled(true);
	}
}
