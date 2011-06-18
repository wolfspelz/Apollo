package com.zweitgeist.tools.langman;

/**
 * Includes all settings/constants of the LangMan project that are hardwired. Constants should not be defined anywhere else!
 * @author jkuehne
 *
 */
public interface Constants {
	
	public final static String VERSION = "0.9.4";
	public final static String SETTINGS_FILE_GLOBAL	= "LangMan.properties";
	public final static String SETTINGS_FILE_LOCAL	= "LangMan.local.properties";
	
	
	// Events
	public final static int EVT_MI_IMPORT_PRESSED				=  0;	
	public final static int EVT_BTN_IMPORT_OK_PRESSED			=  1;
	public final static int EVT_MI_LOAD_CSV_PRESSED				=  2;
	public final static int EVT_MI_LOAD_PATCH_PRESSED			=  3;
	public final static int EVT_MI_SAVE_CSV_PRESSED				=  4;
	public final static int EVT_MI_GENERATE_PRESSED				=  5;
	public final static int EVT_BTN_EXPORT_CANCEL_PRESSED		=  6;
	public final static int EVT_BTN_EXPORT_OK_PRESSED			=  7;
	public final static int EVT_MI_TRANSLATOR_PRESSED			=  8;
	public final static int EVT_BTN_TRANSLATOR_CHOOSE_PRESSED	=  9;
	public final static int EVT_BTN_TRANSLATOR_CANCEL_PRESSED	= 10;
	public final static int EVT_BTN_TRANSLATOR_OK_PRESSED		= 11;
	
	// Columns
	public final static int CLM_PART		= 0;
	public final static int CLM_CHANGED		= 1;
	public final static int CLM_KEY			= 2;
	public final static int CLM_REFERENCE	= 3;
	public final static int CLM_COMMENT		= 4;
	public final static int CLM_LIMIT		= 5;
	public final static String[] S_CLOLUMN_NAMES = { "Part", "Changed", "Key", "Reference", "Comment", "Limit" };
	
	
	// Property keys
	public final static String KEY_CSV_FILE_NAME		= "CsvFile";
	public final static String KEY_TEMPLATE_PATH		= "TemplatePath";
	public final static String KEY_FILE_TEMPLATE		= "FileTemplate";
	public final static String KEY_LINE_TEMPLATE		= "LineTemplate";
	public final static String KEY_LANG					= "LangVariable";
	public final static String KEY_ITEMS				= "ItemsVariable";
	public final static String KEY_KEY					= "KeyVariable";
	public final static String KEY_VAL					= "ValVariable";
	public final static String KEY_EXPORT_PATH			= "ExportPath";	
	public final static String KEY_EXPORT_NAME			= "ExportName";
	public final static String KEY_ENCODING				= "Encoding";
	public final static String KEY_BOUNDARY_CHAR_KEY	= "BoundaryCharKey";
	public final static String KEY_ESCAPE_CHAR_KEY		= "EscapeCharKey";
	public final static String KEY_BOUNDARY_CHAR_VAL	= "BoundaryCharVal";
	public final static String KEY_ESCAPE_CHAR_VAL		= "EscapeCharVal";
	public final static String KEY_DEFAULT_LANG			= "DefaultLang";
	public final static String KEY_EMPTY_STRING			= "EmptyString";
	public final static String KEY_NO_VAL_FOUND			= "NoValFound";
	public final static String KEY_USE_DEFAULT_LANG		= "UseDefaultLang";
	public final static String KEY_RUN_UNIT_TESTS		= "RunUnitTests";
	public final static String KEY_TRANSLATOR_FILE		= "TranslatorFileName";
	public final static String KEY_LOG_FILE				= "LogFile";
	public final static String KEY_LOG_LEVEL			= "LogLevel";
	public final static String KEY_LOG_CHANNEL_CONSOLE	= "Console";
	public final static String KEY_LOG_CHANNEL_FILE		= "File";
	public final static String KEY_ADDITIONAL_TARGET	= "AdditionalTarget";
	
	// Log Levels	
	public final static int LOG_LEVEL_LOG	= 0;	
	public final static int LOG_LEVEL_WARN	= 1;
	public final static int LOG_LEVEL_ERROR	= 2;
	public final static int LOG_LEVEL_STFU	= 3;
	public final static int LOG_LEVEL_MSG	= 4;
	public final static String[] LOG_LEVEL_NAMES = {"log", "warn", "error", "stfu"};
	
	
	
	// Behavior on no val found	
	public final static int NVB_OMMIT			= 0;
	public final static int NVB_EMPTY			= 1;
	public final static int NVB_KEY				= 2;
	public final String[] S_BEHAVIORS = {"ommit", "empty", "key"};
	
	
	// Various
	public final static String PROPERTIES_EXTENSION = "properties";
	

}
