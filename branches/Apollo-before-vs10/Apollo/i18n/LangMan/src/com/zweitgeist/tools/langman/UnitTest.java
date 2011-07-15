package com.zweitgeist.tools.langman;

import java.io.File;

/**
 * Contains all unit tests for the LangMan project
 * @author jkuehne
 *
 */
public class UnitTest {
	
	
	/**
	 * CSV Parser Test
	 */
	private final static String[][] CSV_PARSER_PASS = {
		{"1;\"zwei\";\"drei\";",			"1", "zwei", "drei", ""},
		{"1;2;3;",							"1", "2", "3", ""},
		{"1;2;3",							"1", "2", "3"},				
		{"1;\"zwei\";3",					"1", "zwei", "3"},				
		{"\"eins\";\"zwei\";\"drei\"",		"eins", "zwei", "drei"},
		{"\"eins\";;\"drei\"",				"eins", "", "drei"},
		{"\"eins;zwei\";;\"drei\"",			"eins;zwei", "", "drei"},
		{"\"ei\"\"ns\";\"zwei\";\"drei\"",	"ei\"ns", "zwei", "drei"},
		{"\"ei\"\";ns\";\"zwei\";\"drei\"",	"ei\";ns", "zwei", "drei"},
		{"\"ei\"\";ns\";;\"drei\";4",		"ei\";ns", "", "drei", "4"},
	};
	
	private final static String[] CSV_PARSER_FAIL = {
		"1a;2;3",
		"1;2 ;3",
		"1;\"2;3",
		"eins,2"
	};
	
	
	/**
	 * CSV Encoder Test
	 */
	private final static String[][] CSV_ENCODER = {
		{"2",		"2"},
		{"abc",		"\"abc\""},
		{"ab\"c",	"\"ab\"\"c\""},
		{"",		""},
		{null,		""},
		{"ab;c",	"\"ab;c\""},
		{"2a",		"\"2a\""}
	};
	
	
	
	/**
	 * Tools Test
	 */
	private final static String[] IS_INT_PASS = { "1", "-3", "123", "4", "14" };	
	private final static String[] IS_INT_FAIL = { "", "a", "1.2", "23,4", "one", "zwei" };
	
	private final static String[][] REPLACE_EXTENSION = {
		{"c:\\test.csv",	"csv",	"properties",	"c:\\test.properties"},
		{"c:/test.csv",		"csv",	"properties",	"c:\\test.properties"},
		{"c:\test.csv",		"txt",	"properties",	null},
		{null,				"txt",	"properties",	null},
		{"c:\test.csv",		null, 	"properties",	null},
		{"c:\test.csv",		"txt",	null,			null},		
		{"c:/test",			"csv",	"properties",	null}
	};
	

	private final static String[][] STRING_ESCAPCE = {
		{"test", "'", "+", "test"},
		{"te'st", "'", "+", "te+'st"},
		{"te?'st", "'", "+", "te?+'st"},
		{"te\\'st", "'", "+", "te\\+'st"},
		{"te?\"\\st", "\"", "+", "te?+\"\\st"},
		{"tefst", "f", "\\", "te\\fst"},
		{"te$1st", "'", "+", "te$1st"},
	};
	
	
	
	/**
	 * Is called from the LangMan - if you want to add a unit test add it here.
	 * @return
	 */
	public static boolean testAll(){	
		boolean bResult = true;
		UnitTest test = new UnitTest();
		
		System.out.println("Starting CsvParserTest ...");
		if (test.testCsvParser()){
			System.out.println("... passed CsvParserTest");
		} else {
			System.err.println("... CsvParserTest FAILED!");
			bResult &= false;
		}
		
		System.out.println("Starting CsvEncoderTest ...");
		if (test.testCsvEncoder()){
			System.out.println("... passed CsvEncoderTest");
		}  else {
			System.err.println("... CsvEncoderTest FAILED!");
			bResult &= false;
		}
		
		System.out.println("Starting ToolsTest ...");
		if (test.testTools()){
			System.out.println("... passed ToolsTest");
		} else {
			System.err.println("... ToolsTest FAILED!");
			bResult &= false;
		}
		return bResult;
	}
	
	
	
	private boolean testReplaceExtension(String sFileName, String sOldExtension, String sNewExtension, String sExpectedResultName){
		boolean bResult = true;
		if (sFileName != null){
			File fIn = new File(sFileName);
			File fResult = Tools.replaceExtension(fIn, sOldExtension, sNewExtension);
			if (fResult == null){
				bResult = sExpectedResultName == null;
			} else {
				if (sExpectedResultName == null){
					bResult = false;
				} else {
					bResult = sExpectedResultName.equals(fResult.getAbsolutePath());
				}			
			}
		} else {
			bResult = sExpectedResultName == null;
		}
		return bResult;
	}
	
	
	
	private boolean testTools(){
		boolean bResult = true;
		for(int i=0; i<IS_INT_PASS.length; i++){
			if(!Tools.isInt(IS_INT_PASS[i])){
				System.out.println("Tools: IS_INT_PASS - Test No "+i+" failed: "+IS_INT_PASS[i]);
				bResult &= false;
			}
		}
		for(int i=0; i<IS_INT_FAIL.length; i++){
			if(Tools.isInt(IS_INT_FAIL[i])){
				System.out.println("Tools: IS_INT_FAIL - Test No "+i+" should have failed: "+IS_INT_FAIL[i]);
				bResult &= false;
			}
		}
		for(int i=0; i<REPLACE_EXTENSION.length; i++){
			if(!testReplaceExtension(REPLACE_EXTENSION[i][0], REPLACE_EXTENSION[i][1], REPLACE_EXTENSION[i][2], REPLACE_EXTENSION[i][3])){
				System.out.println("Tools: REPLACE_EXTENSION - Test No "+i+" failed: "+REPLACE_EXTENSION[i][0]);
				bResult &= false;
			}
		}
		for(int i=0; i<STRING_ESCAPCE.length; i++){
			String sResult = Tools.escapeString(STRING_ESCAPCE[i][0], STRING_ESCAPCE[i][1], STRING_ESCAPCE[i][2]); 
			if(!sResult.equals(STRING_ESCAPCE[i][3])){
				System.out.println("Tools: STRING_ESCAPCE - Test No "+i+" failed: expected: "+STRING_ESCAPCE[i][3]+" result: "+sResult);
				bResult &= false;
			}
		}
		return bResult;
	}
	
	
	
	private boolean testCsvParser(){
		boolean bResult = true;
		for(int i=0; i<CSV_PARSER_PASS.length; i++){
			if(!testCsvParserLine(CSV_PARSER_PASS[i])){
				System.out.println("CSV_PARSER_PASS - Test No "+i+" failed: "+CSV_PARSER_PASS[i][0]);
				bResult &= false;
			}
		}
		for(int i=0; i<CSV_PARSER_FAIL.length; i++){
			if(testCsvParserLine( new String[]{CSV_PARSER_FAIL[i]})){
				System.out.println("CSV_PARSER_FAIL - Test No "+i+" should have failed: "+CSV_PARSER_FAIL[i]);
				bResult &= false;
			}
		}		
		return bResult;
	}
	
	
	
	private boolean testCsvParserLine(String[] aCsvIn){
		boolean bResult = true;
		String[] aCsvOut = Tools.parseCsv(aCsvIn[0]);
		if( aCsvOut != null){
			bResult = aCsvIn.length == aCsvOut.length+1;
			int i=0;
			while(bResult && aCsvOut!= null &&  i<aCsvOut.length){
				bResult &= aCsvOut[i].equals(aCsvIn[i+1]);
				i++;				
			}
			return bResult;
		} else {
			bResult = false;
		}
		return bResult;
	}
	
	
	
	private boolean testCsvEncoder(){
		boolean bResult = true;
		for(int i=0; i<CSV_ENCODER.length; i++){
			String sTemp = Tools.encodeCsv(CSV_ENCODER[i][0]);
			if( !sTemp.equals(CSV_ENCODER[i][1])){
				System.out.println("CSV_ENCODER - Test No "+i+" failed: in='"+CSV_ENCODER[i][0]+"'  out='"+sTemp+"'  expected='"+CSV_ENCODER[i][1]+"'");
				bResult &= false;
			}
		}
		return bResult;
	}

}
