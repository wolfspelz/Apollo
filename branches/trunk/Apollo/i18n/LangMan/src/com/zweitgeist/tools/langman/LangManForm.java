package com.zweitgeist.tools.langman;

import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import java.awt.BorderLayout;
import javax.swing.SwingConstants;
import java.awt.Point;

import javax.swing.ButtonGroup;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JMenuItem;
import javax.swing.JMenuBar;
import javax.swing.JMenu;
import javax.swing.JFrame;
import javax.swing.JDialog;
import java.awt.GridBagLayout;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import java.awt.GridBagConstraints;
import javax.swing.JButton;
import java.awt.Dimension;
import javax.swing.BorderFactory;
import javax.swing.border.TitledBorder;
import java.awt.Font;
import java.awt.Color;
import java.io.File;

import javax.swing.JTextField;
import javax.swing.JComboBox;
import javax.swing.JRadioButton;
import javax.swing.JTabbedPane;
import javax.swing.JList;
import javax.swing.ListSelectionModel;

/**
 * Contains all gui elements of the LangMan project. All events are handled in the parent (LangMan) that holds the
 * LangManForm 
 * @author jkuehne
 *
 */
public class LangManForm implements Constants {
	
	private LangMan parent;

	protected JFrame frmMain = null;  //  @jve:decl-index=0:visual-constraint="92,64"

	private JMenuBar mbMain = null;

	private JMenu mFile = null;

	private JMenu mHelp = null;

	private JMenuItem miExit = null;

	private JMenuItem aboutMenuItem = null;

	private JDialog aboutDialog = null;

	private JPanel aboutContentPane = null;

	private JLabel aboutVersionLabel = null;

	private JMenuItem miImport = null;

	private JPanel pnlMain = null;

	private JScrollPane spMain = null;

	protected JTable tblMain = null;

	private JDialog dlgImport = null;  //  @jve:decl-index=0:visual-constraint="1099,246"

	private JPanel cpDlgImport = null;

	private JLabel lblFileName = null;

	protected JTextField tfFileName = null;

	private JButton btnFile = null;

	private JPanel pnlButtons = null;

	private JButton btnOk = null;

	private JButton btnCancel = null;

	private JLabel lblFormat = null;

	protected JComboBox cmbxFormats = null;

	private JLabel lblAddTo = null;

	private JPanel pnlAddTo = null;

	protected JTextField tfLangId = null;

	protected JComboBox cmbxExistingLanguages = null;

	private JPanel pnlNewLanguage = null;

	private JPanel pnlExistingLanguage = null;

	private JLabel lblLangShortName = null;

	private JLabel lblLongName = null;

	protected JTextField tfLangName = null;

	private JTabbedPane tpNewLanguage = null;

	private JLabel lblLanguage = null;

	private JPanel pnlAddingPolicy = null;

	protected JRadioButton rbAddMissing = null;

	protected JRadioButton rbOverwrite = null;

	private JMenuItem miLoadCvs = null;

	protected JMenuItem miGenerate = null;

	private JDialog dlgGenerate = null;  //  @jve:decl-index=0:visual-constraint="632,196"

	private JPanel pnlGenerateMain = null;

	private JScrollPane spGenreateLangs = null;

	private JScrollPane spGenerateParts = null;

	protected JList lstGenerateLangs = null;

	protected JList lstGenerateParts = null;

	private JPanel pnlGenerateButtons = null;

	private JButton btnGenerateOk = null;

	private JButton btnGenerateCancel = null;

	protected JMenuItem miLoadPatch = null;

	private JMenuItem miSaveCsv = null;

	private JPanel pnlGenerateLang = null;

	private JPanel pnlGenerateParts = null;

	private JMenuItem miTranslatorFile = null;

	private JDialog dlgTranslator = null;  //  @jve:decl-index=0:visual-constraint="628,0"

	private JPanel pnlTranslatorMain = null;

	private JPanel pnlTranslatorSources = null;

	private JPanel pnlTranslatorTargets = null;

	private JPanel pnlTranslatorParts = null;

	private JPanel pnlTranslatorButtons = null;

	private JButton btnTranslatorCancel = null;

	private JButton btnTranslatorOk = null;

	private JScrollPane spTranslatorSources = null;

	private JScrollPane spTranslatorTargets = null;

	private JScrollPane spTranslatorParts = null;

	private JList lstTranslatorSources = null;

	private JList lstTranslatorTargets = null;

	private JList lstTranslatorParts = null;

	private JPanel pnlTranslatorDir = null;

	private JTextField tfTranslatorFile = null;

	private JButton btnTranslatorChoose = null;

	public LangManForm( LangMan parent ){
		super();
		this.parent = parent;
	}

	/**
	 * This method initializes miImport	
	 * 	
	 * @return javax.swing.JMenuItem	
	 */
	private JMenuItem getMiImport() {
		if (miImport == null) {
			miImport = new JMenuItem();
			miImport.setText("Import");
			miImport.setVisible(false);
			miImport.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					parent.handleEvents(EVT_MI_IMPORT_PRESSED);
				}
			});
		}
		return miImport;
	}

	/**
	 * @param args
	 */
//	public static void main(String[] args) {
//		// TODO Auto-generated method stub
//		SwingUtilities.invokeLater(new Runnable() {
//			public void run() {
//				LanguageManagerForm application = new LanguageManagerForm();
//				application.getJFrame().setVisible(true);
//			}
//		});
//	}

	/**
	 * This method initializes frmMain
	 * 
	 * @return javax.swing.JFrame
	 */
	protected JFrame getFrmMain() {
		if (frmMain == null) {
			frmMain = new JFrame();
			frmMain.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
			frmMain.setJMenuBar(getMbMain());
			frmMain.setSize(505, 407);
			frmMain.setContentPane(getPnlMain());
			frmMain.setTitle("LanguageManager");
		}
		return frmMain;
	}

	/**
	 * This method initializes mbMain	
	 * 	
	 * @return javax.swing.JMenuBar	
	 */
	private JMenuBar getMbMain() {
		if (mbMain == null) {
			mbMain = new JMenuBar();
			mbMain.add(getMFile());
			mbMain.add(getMHelp());
		}
		return mbMain;
	}

	/**
	 * This method initializes jMenu	
	 * 	
	 * @return javax.swing.JMenu	
	 */
	private JMenu getMFile() {
		if (mFile == null) {
			mFile = new JMenu();
			mFile.setText("File");
			mFile.add(getMiLoadCvs());
			mFile.add(getMiLoadPatch());
			mFile.add(getMiSaveCsv());
			mFile.add(getMiGenerate());
			mFile.add(getMiImport());
			mFile.add(getMiTranslatorFile());
			mFile.add(getMiExit());
		}
		return mFile;
	}

	/**
	 * This method initializes jMenu	
	 * 	
	 * @return javax.swing.JMenu	
	 */
	private JMenu getMHelp() {
		if (mHelp == null) {
			mHelp = new JMenu();
			mHelp.setText("Help");
			mHelp.add(getAboutMenuItem());
		}
		return mHelp;
	}

	/**
	 * This method initializes jMenuItem	
	 * 	
	 * @return javax.swing.JMenuItem	
	 */
	private JMenuItem getMiExit() {
		if (miExit == null) {
			miExit = new JMenuItem();
			miExit.setText("Exit");
			miExit.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					System.exit(0);
				}
			});
		}
		return miExit;
	}

	/**
	 * This method initializes jMenuItem	
	 * 	
	 * @return javax.swing.JMenuItem	
	 */
	private JMenuItem getAboutMenuItem() {
		if (aboutMenuItem == null) {
			aboutMenuItem = new JMenuItem();
			aboutMenuItem.setText("About");
			aboutMenuItem.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					JDialog aboutDialog = getAboutDialog();
					aboutDialog.pack();
					Point loc = getFrmMain().getLocation();
					loc.translate(20, 20);
					aboutDialog.setLocation(loc);
					aboutDialog.setVisible(true);
				}
			});
		}
		return aboutMenuItem;
	}

	/**
	 * This method initializes aboutDialog	
	 * 	
	 * @return javax.swing.JDialog
	 */
	private JDialog getAboutDialog() {
		if (aboutDialog == null) {
			aboutDialog = new JDialog(getFrmMain(), true);
			aboutDialog.setTitle("About");
			aboutDialog.setContentPane(getAboutContentPane());
		}
		return aboutDialog;
	}

	/**
	 * This method initializes aboutContentPane
	 * 
	 * @return javax.swing.JPanel
	 */
	private JPanel getAboutContentPane() {
		if (aboutContentPane == null) {
			aboutContentPane = new JPanel();
			aboutContentPane.setLayout(new BorderLayout());
			aboutContentPane.add(getAboutVersionLabel(), BorderLayout.CENTER);
		}
		return aboutContentPane;
	}

	/**
	 * This method initializes aboutVersionLabel	
	 * 	
	 * @return javax.swing.JLabel	
	 */
	private JLabel getAboutVersionLabel() {
		if (aboutVersionLabel == null) {
			aboutVersionLabel = new JLabel();
			aboutVersionLabel.setText("Weblin forever!");
			aboutVersionLabel.setHorizontalAlignment(SwingConstants.CENTER);
		}
		return aboutVersionLabel;
	}

	/**
	 * This method initializes pnlMain	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getPnlMain() {
		if (pnlMain == null) {
			GridBagConstraints gridBagConstraints = new GridBagConstraints();
			gridBagConstraints.fill = GridBagConstraints.BOTH;
			gridBagConstraints.weighty = 1.0;
			gridBagConstraints.weightx = 1.0;
			pnlMain = new JPanel();
			pnlMain.setLayout(new GridBagLayout());
			pnlMain.add(getSpMain(), gridBagConstraints);
		}
		return pnlMain;
	}

	/**
	 * This method initializes spMain	
	 * 	
	 * @return javax.swing.JScrollPane	
	 */
	private JScrollPane getSpMain() {
		if (spMain == null) {
			spMain = new JScrollPane();
			spMain.setViewportView(getTblMain());
		}
		return spMain;
	}

	/**
	 * This method initializes tblMain	
	 * 	
	 * @return javax.swing.JTable	
	 */
	private JTable getTblMain() {
		if (tblMain == null) {
			tblMain = new JTable();
		}
		return tblMain;
	}

	/**
	 * This method initializes dlgImport	
	 * 	
	 * @return javax.swing.JDialog	
	 */
	protected JDialog getDlgImport() {
		if (dlgImport == null) {
			dlgImport = new JDialog(getFrmMain());
			dlgImport.setTitle("Import");
			dlgImport.setSize(new Dimension(549, 352));
			dlgImport.setMinimumSize(new Dimension(400, 257));
			dlgImport.setContentPane(getCpDlgImport());
		}
		return dlgImport;
	}

	/**
	 * This method initializes cpDlgImport	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getCpDlgImport() {
		if (cpDlgImport == null) {
			GridBagConstraints gridBagConstraints22 = new GridBagConstraints();
			gridBagConstraints22.fill = GridBagConstraints.BOTH;
			gridBagConstraints22.gridy = 2;
			gridBagConstraints22.weightx = 1.0;
			gridBagConstraints22.weighty = 1.0;
			gridBagConstraints22.gridwidth = 2;
			gridBagConstraints22.gridx = 1;
			GridBagConstraints gridBagConstraints9 = new GridBagConstraints();
			gridBagConstraints9.gridx = 1;
			gridBagConstraints9.fill = GridBagConstraints.HORIZONTAL;
			gridBagConstraints9.gridwidth = 2;
			gridBagConstraints9.gridy = 3;
			GridBagConstraints gridBagConstraints10 = new GridBagConstraints();
			gridBagConstraints10.gridx = 0;
			gridBagConstraints10.anchor = GridBagConstraints.NORTHWEST;
			gridBagConstraints10.gridy = 2;
			lblAddTo = new JLabel();
			lblAddTo.setText("Add To:");
			GridBagConstraints gridBagConstraints8 = new GridBagConstraints();
			gridBagConstraints8.fill = GridBagConstraints.BOTH;
			gridBagConstraints8.gridy = 1;
			gridBagConstraints8.weightx = 1.0;
			gridBagConstraints8.gridwidth = 2;
			gridBagConstraints8.gridx = 1;
			GridBagConstraints gridBagConstraints7 = new GridBagConstraints();
			gridBagConstraints7.gridx = 0;
			gridBagConstraints7.anchor = GridBagConstraints.WEST;
			gridBagConstraints7.gridy = 1;
			lblFormat = new JLabel();
			lblFormat.setText("Format:");
			GridBagConstraints gridBagConstraints4 = new GridBagConstraints();
			gridBagConstraints4.gridx = 0;
			gridBagConstraints4.fill = GridBagConstraints.NONE;
			gridBagConstraints4.weighty = 0.1;
			gridBagConstraints4.gridwidth = 3;
			gridBagConstraints4.anchor = GridBagConstraints.SOUTHEAST;
			gridBagConstraints4.gridy = 4;
			GridBagConstraints gridBagConstraints3 = new GridBagConstraints();
			gridBagConstraints3.gridx = 2;
			gridBagConstraints3.gridy = 0;
			GridBagConstraints gridBagConstraints2 = new GridBagConstraints();
			gridBagConstraints2.fill = GridBagConstraints.HORIZONTAL;
			gridBagConstraints2.gridy = 0;
			gridBagConstraints2.weightx = 0.1;
			gridBagConstraints2.weighty = 0.0;
			gridBagConstraints2.gridx = 1;
			GridBagConstraints gridBagConstraints1 = new GridBagConstraints();
			gridBagConstraints1.gridx = 0;
			gridBagConstraints1.anchor = GridBagConstraints.WEST;
			gridBagConstraints1.gridy = 0;
			lblFileName = new JLabel();
			lblFileName.setText("File:");
			cpDlgImport = new JPanel();
			cpDlgImport.setLayout(new GridBagLayout());
			cpDlgImport.add(lblFileName, gridBagConstraints1);
			cpDlgImport.add(getTfFileName(), gridBagConstraints2);
			cpDlgImport.add(getBtnFile(), gridBagConstraints3);
			cpDlgImport.add(getPnlButtons(), gridBagConstraints4);
			cpDlgImport.add(lblFormat, gridBagConstraints7);
			cpDlgImport.add(getCmbxFormats(), gridBagConstraints8);
			cpDlgImport.add(lblAddTo, gridBagConstraints10);
			cpDlgImport.add(getPnlAddTo(), gridBagConstraints9);
			cpDlgImport.add(getTpNewLanguage(), gridBagConstraints22);
			
			ButtonGroup btngrpOverwrite = new ButtonGroup();
			btngrpOverwrite.add( rbAddMissing );
			btngrpOverwrite.add( rbOverwrite );
		}
		return cpDlgImport;
	}

	/**
	 * This method initializes tfFileName	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getTfFileName() {
		if (tfFileName == null) {
			tfFileName = new JTextField();
		}
		return tfFileName;
	}

	/**
	 * This method initializes btnFile	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getBtnFile() {
		if (btnFile == null) {
			btnFile = new JButton();
			btnFile.setText("Choose File");
			btnFile.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					JFileChooser fc = new JFileChooser();
					int returnVal = fc.showOpenDialog( dlgImport );
					if (returnVal == JFileChooser.APPROVE_OPTION) {
			            File file = fc.getSelectedFile();
			            tfFileName.setText( file.getAbsolutePath() );
			        }

				}
			});
		}
		return btnFile;
	}

	/**
	 * This method initializes pnlButtons	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getPnlButtons() {
		if (pnlButtons == null) {
			GridBagConstraints gridBagConstraints6 = new GridBagConstraints();
			gridBagConstraints6.anchor = GridBagConstraints.EAST;
			GridBagConstraints gridBagConstraints5 = new GridBagConstraints();
			gridBagConstraints5.anchor = GridBagConstraints.EAST;
			pnlButtons = new JPanel();
			pnlButtons.setLayout(new GridBagLayout());
			pnlButtons.add(getBtnCancel(), gridBagConstraints6);
			pnlButtons.add(getBtnOk(), gridBagConstraints5);
		}
		return pnlButtons;
	}

	/**
	 * This method initializes btnOk	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getBtnOk() {
		if (btnOk == null) {
			btnOk = new JButton();
			btnOk.setText("OK");
			btnOk.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					parent.handleEvents( EVT_BTN_IMPORT_OK_PRESSED );
				}
			});
		}
		return btnOk;
	}

	/**
	 * This method initializes btnCancel	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getBtnCancel() {
		if (btnCancel == null) {
			btnCancel = new JButton();
			btnCancel.setText("Cancel");
		}
		return btnCancel;
	}

	/**
	 * This method initializes cmbxFormats	
	 * 	
	 * @return javax.swing.JComboBox	
	 */
	private JComboBox getCmbxFormats() {
		if (cmbxFormats == null) {
			cmbxFormats = new JComboBox();
//			ComboBoxModel model = new DefaultComboBoxModel( FILE_FORMAT_NAMES );
//			cmbxFormats.setModel( model );
		}
		return cmbxFormats;
	}

	/**
	 * This method initializes pnlAddTo	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getPnlAddTo() {
		if (pnlAddTo == null) {
			GridBagConstraints gridBagConstraints15 = new GridBagConstraints();
			gridBagConstraints15.gridx = 0;
			gridBagConstraints15.anchor = GridBagConstraints.NORTHWEST;
			gridBagConstraints15.fill = GridBagConstraints.HORIZONTAL;
			gridBagConstraints15.weighty = 0.1;
			gridBagConstraints15.weightx = 0.1;
			gridBagConstraints15.gridy = 1;
			pnlAddTo = new JPanel();
			pnlAddTo.setLayout(new GridBagLayout());
			pnlAddTo.setEnabled(true);
		}
		return pnlAddTo;
	}

	/**
	 * This method initializes tfLangId	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	private JTextField getTfLangId() {
		if (tfLangId == null) {
			tfLangId = new JTextField();
		}
		return tfLangId;
	}

	/**
	 * This method initializes cmbxExistingLanguages	
	 * 	
	 * @return javax.swing.JComboBox	
	 */
	private JComboBox getCmbxExistingLanguages() {
		if (cmbxExistingLanguages == null) {
			cmbxExistingLanguages = new JComboBox();
			cmbxExistingLanguages.setEnabled(true);
		}
		return cmbxExistingLanguages;
	}

	/**
	 * This method initializes pnlNewLanguage	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getPnlNewLanguage() {
		if (pnlNewLanguage == null) {
			GridBagConstraints gridBagConstraints21 = new GridBagConstraints();
			gridBagConstraints21.fill = GridBagConstraints.HORIZONTAL;
			gridBagConstraints21.gridy = 2;
			gridBagConstraints21.weightx = 1.0;
			gridBagConstraints21.weighty = 0.1;
			gridBagConstraints21.anchor = GridBagConstraints.NORTHWEST;
			gridBagConstraints21.gridx = 2;
			GridBagConstraints gridBagConstraints20 = new GridBagConstraints();
			gridBagConstraints20.gridx = 1;
			gridBagConstraints20.anchor = GridBagConstraints.NORTHWEST;
			gridBagConstraints20.gridy = 2;
			lblLongName = new JLabel();
			lblLongName.setText("Name:");
			GridBagConstraints gridBagConstraints19 = new GridBagConstraints();
			gridBagConstraints19.gridx = 1;
			gridBagConstraints19.anchor = GridBagConstraints.WEST;
			gridBagConstraints19.gridy = 1;
			lblLangShortName = new JLabel();
			lblLangShortName.setText("Id:");
			GridBagConstraints gridBagConstraints12 = new GridBagConstraints();
			gridBagConstraints12.fill = GridBagConstraints.BOTH;
			gridBagConstraints12.gridy = 1;
			gridBagConstraints12.weightx = 1.0;
			gridBagConstraints12.gridwidth = 1;
			gridBagConstraints12.gridx = 2;
			GridBagConstraints gridBagConstraints11 = new GridBagConstraints();
			gridBagConstraints11.anchor = GridBagConstraints.WEST;
			gridBagConstraints11.gridy = -1;
			gridBagConstraints11.gridx = -1;
			pnlNewLanguage = new JPanel();
			pnlNewLanguage.setLayout(new GridBagLayout());
			pnlNewLanguage.setBorder(BorderFactory.createEmptyBorder(0, 0, 0, 0));
			pnlNewLanguage.add(getTfLangId(), gridBagConstraints12);
			pnlNewLanguage.add(lblLangShortName, gridBagConstraints19);
			pnlNewLanguage.add(lblLongName, gridBagConstraints20);
			pnlNewLanguage.add(getTfLangName(), gridBagConstraints21);
		}
		return pnlNewLanguage;
	}

	/**
	 * This method initializes pnlExistingLanguage	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getPnlExistingLanguage() {
		if (pnlExistingLanguage == null) {
			GridBagConstraints gridBagConstraints17 = new GridBagConstraints();
			gridBagConstraints17.gridx = 0;
			gridBagConstraints17.gridwidth = 2;
			gridBagConstraints17.fill = GridBagConstraints.HORIZONTAL;
			gridBagConstraints17.weighty = 0.1;
			gridBagConstraints17.anchor = GridBagConstraints.NORTHWEST;
			gridBagConstraints17.gridy = 1;
			GridBagConstraints gridBagConstraints18 = new GridBagConstraints();
			gridBagConstraints18.gridx = 0;
			gridBagConstraints18.gridy = 0;
			lblLanguage = new JLabel();
			lblLanguage.setText("Language:");
			GridBagConstraints gridBagConstraints16 = new GridBagConstraints();
			gridBagConstraints16.anchor = GridBagConstraints.WEST;
			gridBagConstraints16.gridy = 0;
			gridBagConstraints16.fill = GridBagConstraints.HORIZONTAL;
			gridBagConstraints16.weightx = 0.1;
			gridBagConstraints16.gridx = 0;
			GridBagConstraints gridBagConstraints14 = new GridBagConstraints();
			gridBagConstraints14.fill = GridBagConstraints.BOTH;
			gridBagConstraints14.gridy = -1;
			gridBagConstraints14.weightx = 1.0;
			gridBagConstraints14.anchor = GridBagConstraints.NORTHWEST;
			gridBagConstraints14.gridx = 1;
			GridBagConstraints gridBagConstraints13 = new GridBagConstraints();
			gridBagConstraints13.anchor = GridBagConstraints.WEST;
			gridBagConstraints13.gridy = -1;
			gridBagConstraints13.gridx = -1;
			pnlExistingLanguage = new JPanel();
			pnlExistingLanguage.setLayout(new GridBagLayout());
			pnlExistingLanguage.setBorder(BorderFactory.createEmptyBorder(0, 0, 0, 0));
			pnlExistingLanguage.add(getCmbxExistingLanguages(), gridBagConstraints14);
			pnlExistingLanguage.add(lblLanguage, gridBagConstraints18);
			pnlExistingLanguage.add(getPnlAddingPolicy(), gridBagConstraints17);
		}
		return pnlExistingLanguage;
	}

	/**
	 * This method initializes tfLangName	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	protected JTextField getTfLangName() {
		if (tfLangName == null) {
			tfLangName = new JTextField();
		}
		return tfLangName;
	}

	/**
	 * This method initializes tpNewLanguage	
	 * 	
	 * @return javax.swing.JTabbedPane	
	 */
	private JTabbedPane getTpNewLanguage() {
		if (tpNewLanguage == null) {
			tpNewLanguage = new JTabbedPane();
			tpNewLanguage.addTab("New Language", null, getPnlNewLanguage(), null);
			tpNewLanguage.addTab("Existing Language", null, getPnlExistingLanguage(), null);
		}
		return tpNewLanguage;
	}

	/**
	 * This method initializes pnlAddingPolicy	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getPnlAddingPolicy() {
		if (pnlAddingPolicy == null) {
			GridBagConstraints gridBagConstraints151 = new GridBagConstraints();
			gridBagConstraints151.anchor = GridBagConstraints.NORTHWEST;
			gridBagConstraints151.gridx = 0;
			gridBagConstraints151.gridy = 1;
			gridBagConstraints151.weightx = 0.1;
			gridBagConstraints151.weighty = 0.1;
			gridBagConstraints151.fill = GridBagConstraints.HORIZONTAL;
			GridBagConstraints gridBagConstraints161 = new GridBagConstraints();
			gridBagConstraints161.anchor = GridBagConstraints.WEST;
			gridBagConstraints161.gridx = 0;
			gridBagConstraints161.gridy = 0;
			gridBagConstraints161.weightx = 0.1;
			gridBagConstraints161.fill = GridBagConstraints.HORIZONTAL;
			pnlAddingPolicy = new JPanel();
			pnlAddingPolicy.setLayout(new GridBagLayout());
			pnlAddingPolicy.setBorder(BorderFactory.createTitledBorder(null, "Adding Policy", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, new Font("Dialog", Font.BOLD, 12), new Color(51, 51, 51)));
			pnlAddingPolicy.add(getRbAddMissing(), gridBagConstraints161);
			pnlAddingPolicy.add(getRbOverwrite(), gridBagConstraints151);
		}
		return pnlAddingPolicy;
	}

	/**
	 * This method initializes rbAddMissing	
	 * 	
	 * @return javax.swing.JRadioButton	
	 */
	private JRadioButton getRbAddMissing() {
		if (rbAddMissing == null) {
			rbAddMissing = new JRadioButton();
			rbAddMissing.setEnabled(true);
			rbAddMissing.setText("Add missing items");
			rbAddMissing.setSelected(true);
		}
		return rbAddMissing;
	}

	/**
	 * This method initializes rbOverwrite	
	 * 	
	 * @return javax.swing.JRadioButton	
	 */
	private JRadioButton getRbOverwrite() {
		if (rbOverwrite == null) {
			rbOverwrite = new JRadioButton();
			rbOverwrite.setEnabled(true);
			rbOverwrite.setText("Overwrite existing items");
		}
		return rbOverwrite;
	}

	/**
	 * This method initializes miLoadCvs	
	 * 	
	 * @return javax.swing.JMenuItem	
	 */
	private JMenuItem getMiLoadCvs() {
		if (miLoadCvs == null) {
			miLoadCvs = new JMenuItem();
			miLoadCvs.setText("Load CSV File");
			miLoadCvs.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					parent.handleEvents(EVT_MI_LOAD_CSV_PRESSED);
				}
			});
		}
		return miLoadCvs;
	}

	/**
	 * This method initializes miGenerate	
	 * 	
	 * @return javax.swing.JMenuItem	
	 */
	private JMenuItem getMiGenerate() {
		if (miGenerate == null) {
			miGenerate = new JMenuItem();
			miGenerate.setText("Generate");
			miGenerate.setEnabled(true);
			miGenerate.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					parent.handleEvents(EVT_MI_GENERATE_PRESSED);
				}
			});
		}
		return miGenerate;
	}

	/**
	 * This method initializes dlgGenerate	
	 * 	
	 * @return javax.swing.JDialog	
	 */
	protected JDialog getDlgGenerate() {
		if (dlgGenerate == null) {
			dlgGenerate = new JDialog(getFrmMain());
			dlgGenerate.setTitle("Generate");
			dlgGenerate.setSize(new Dimension(361, 180));
			dlgGenerate.setModal(true);
			dlgGenerate.setMinimumSize(new Dimension(280, 108));
			dlgGenerate.setContentPane(getPnlGenerateMain());
		}
		return dlgGenerate;
	}

	/**
	 * This method initializes pnlGenerateMain	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getPnlGenerateMain() {
		if (pnlGenerateMain == null) {
			GridBagConstraints gridBagConstraints27 = new GridBagConstraints();
			gridBagConstraints27.fill = GridBagConstraints.BOTH;
			gridBagConstraints27.weighty = 0.1;
			gridBagConstraints27.weightx = 0.1;
			GridBagConstraints gridBagConstraints26 = new GridBagConstraints();
			gridBagConstraints26.fill = GridBagConstraints.BOTH;
			gridBagConstraints26.weighty = 0.1;
			gridBagConstraints26.weightx = 0.1;
			GridBagConstraints gridBagConstraints25 = new GridBagConstraints();
			gridBagConstraints25.gridx = 0;
			gridBagConstraints25.anchor = GridBagConstraints.EAST;
			gridBagConstraints25.gridwidth = 2;
			gridBagConstraints25.gridy = 1;
			pnlGenerateMain = new JPanel();
			pnlGenerateMain.setLayout(new GridBagLayout());
			pnlGenerateMain.add(getPnlGenerateButtons(), gridBagConstraints25);
			pnlGenerateMain.add(getPnlGenerateLang(), gridBagConstraints26);
			pnlGenerateMain.add(getPnlGenerateParts(), gridBagConstraints27);
		}
		return pnlGenerateMain;
	}

	/**
	 * This method initializes spGenreateLangs	
	 * 	
	 * @return javax.swing.JScrollPane	
	 */
	private JScrollPane getSpGenreateLangs() {
		if (spGenreateLangs == null) {
			spGenreateLangs = new JScrollPane();
			spGenreateLangs.setViewportView(getLstGenerateLangs());
		}
		return spGenreateLangs;
	}

	/**
	 * This method initializes spGenerateParts	
	 * 	
	 * @return javax.swing.JScrollPane	
	 */
	private JScrollPane getSpGenerateParts() {
		if (spGenerateParts == null) {
			spGenerateParts = new JScrollPane();
			spGenerateParts.setViewportView(getLstGenerateParts());
		}
		return spGenerateParts;
	}

	/**
	 * This method initializes lstGenerateLangs	
	 * 	
	 * @return javax.swing.JList	
	 */
	private JList getLstGenerateLangs() {
		if (lstGenerateLangs == null) {
			lstGenerateLangs = new JList();
		}
		return lstGenerateLangs;
	}

	/**
	 * This method initializes lstGenerateParts	
	 * 	
	 * @return javax.swing.JList	
	 */
	private JList getLstGenerateParts() {
		if (lstGenerateParts == null) {
			lstGenerateParts = new JList();
		}
		return lstGenerateParts;
	}

	/**
	 * This method initializes pnlGenerateButtons	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getPnlGenerateButtons() {
		if (pnlGenerateButtons == null) {
			pnlGenerateButtons = new JPanel();
			pnlGenerateButtons.setLayout(new GridBagLayout());
			pnlGenerateButtons.add(getBtnGenerateCancel(), new GridBagConstraints());
			pnlGenerateButtons.add(getBtnGenerateOk(), new GridBagConstraints());
		}
		return pnlGenerateButtons;
	}

	/**
	 * This method initializes btnGenerateOk	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getBtnGenerateOk() {
		if (btnGenerateOk == null) {
			btnGenerateOk = new JButton();
			btnGenerateOk.setText("Ok");
			btnGenerateOk.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					parent.handleEvents(EVT_BTN_EXPORT_OK_PRESSED);
				}
			});
		}
		return btnGenerateOk;
	}

	/**
	 * This method initializes btnGenerateCancel	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getBtnGenerateCancel() {
		if (btnGenerateCancel == null) {
			btnGenerateCancel = new JButton();
			btnGenerateCancel.setText("Cancel");
			btnGenerateCancel.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					parent.handleEvents(EVT_BTN_EXPORT_CANCEL_PRESSED);
				}
			});
		}
		return btnGenerateCancel;
	}

	/**
	 * This method initializes miLoadPatch	
	 * 	
	 * @return javax.swing.JMenuItem	
	 */
	private JMenuItem getMiLoadPatch() {
		if (miLoadPatch == null) {
			miLoadPatch = new JMenuItem();
			miLoadPatch.setText("Load Patchfile");
			miLoadPatch.setEnabled(false);
			miLoadPatch.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					parent.handleEvents(EVT_MI_LOAD_PATCH_PRESSED);
				}
			});
		}
		return miLoadPatch;
	}

	/**
	 * This method initializes miSaveCsv	
	 * 	
	 * @return javax.swing.JMenuItem	
	 */
	private JMenuItem getMiSaveCsv() {
		if (miSaveCsv == null) {
			miSaveCsv = new JMenuItem();
			miSaveCsv.setText("Save CSV");
			miSaveCsv.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					parent.handleEvents(EVT_MI_SAVE_CSV_PRESSED);
				}
			});
		}
		return miSaveCsv;
	}

	/**
	 * This method initializes pnlGenerateLang	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getPnlGenerateLang() {
		if (pnlGenerateLang == null) {
			GridBagConstraints gridBagConstraints23 = new GridBagConstraints();
			gridBagConstraints23.fill = GridBagConstraints.BOTH;
			gridBagConstraints23.gridy = -1;
			gridBagConstraints23.weightx = 0.1;
			gridBagConstraints23.weighty = 0.1;
			gridBagConstraints23.gridx = -1;
			pnlGenerateLang = new JPanel();
			pnlGenerateLang.setLayout(new GridBagLayout());
			pnlGenerateLang.setBorder(BorderFactory.createTitledBorder(null, "Languages", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, new Font("Dialog", Font.BOLD, 12), new Color(51, 51, 51)));
			pnlGenerateLang.add(getSpGenreateLangs(), gridBagConstraints23);
		}
		return pnlGenerateLang;
	}

	/**
	 * This method initializes pnlGenerateParts	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getPnlGenerateParts() {
		if (pnlGenerateParts == null) {
			GridBagConstraints gridBagConstraints24 = new GridBagConstraints();
			gridBagConstraints24.fill = GridBagConstraints.BOTH;
			gridBagConstraints24.gridy = -1;
			gridBagConstraints24.weightx = 0.1;
			gridBagConstraints24.weighty = 0.1;
			gridBagConstraints24.gridx = -1;
			pnlGenerateParts = new JPanel();
			pnlGenerateParts.setLayout(new GridBagLayout());
			pnlGenerateParts.setBorder(BorderFactory.createTitledBorder(null, "Parts", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, new Font("Dialog", Font.BOLD, 12), new Color(51, 51, 51)));
			pnlGenerateParts.add(getSpGenerateParts(), gridBagConstraints24);
		}
		return pnlGenerateParts;
	}

	/**
	 * This method initializes miTranslatorFile	
	 * 	
	 * @return javax.swing.JMenuItem	
	 */
	private JMenuItem getMiTranslatorFile() {
		if (miTranslatorFile == null) {
			miTranslatorFile = new JMenuItem();
			miTranslatorFile.setText("Create Translator Files");
			miTranslatorFile.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					parent.handleEvents(EVT_MI_TRANSLATOR_PRESSED);
				}
			});
		}
		return miTranslatorFile;
	}

	/**
	 * This method initializes dlgTranslator	
	 * 	
	 * @return javax.swing.JDialog	
	 */
	protected JDialog getDlgTranslator() {
		if (dlgTranslator == null) {
			dlgTranslator = new JDialog(getFrmMain());
			dlgTranslator.setTitle("Create Translator Files");
			dlgTranslator.setSize(new Dimension(465, 213));
			dlgTranslator.setMinimumSize(new Dimension(460, 200));
			dlgTranslator.setContentPane(getPnlTranslatorMain());
		}
		return dlgTranslator;
	}

	/**
	 * This method initializes pnlTranslatorMain	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getPnlTranslatorMain() {
		if (pnlTranslatorMain == null) {
			GridBagConstraints gridBagConstraints37 = new GridBagConstraints();
			gridBagConstraints37.gridx = 0;
			gridBagConstraints37.fill = GridBagConstraints.HORIZONTAL;
			gridBagConstraints37.gridwidth = 3;
			gridBagConstraints37.weightx = 0.0;
			gridBagConstraints37.gridy = 1;
			GridBagConstraints gridBagConstraints31 = new GridBagConstraints();
			gridBagConstraints31.gridx = 0;
			gridBagConstraints31.gridwidth = 3;
			gridBagConstraints31.fill = GridBagConstraints.NONE;
			gridBagConstraints31.weighty = 0.0;
			gridBagConstraints31.weightx = 0.0;
			gridBagConstraints31.anchor = GridBagConstraints.EAST;
			gridBagConstraints31.gridy = 2;
			GridBagConstraints gridBagConstraints30 = new GridBagConstraints();
			gridBagConstraints30.gridx = 2;
			gridBagConstraints30.weightx = 0.1;
			gridBagConstraints30.weighty = 0.1;
			gridBagConstraints30.fill = GridBagConstraints.BOTH;
			gridBagConstraints30.gridy = 0;
			GridBagConstraints gridBagConstraints29 = new GridBagConstraints();
			gridBagConstraints29.gridx = 1;
			gridBagConstraints29.fill = GridBagConstraints.BOTH;
			gridBagConstraints29.weightx = 0.1;
			gridBagConstraints29.weighty = 0.1;
			gridBagConstraints29.gridy = 0;
			GridBagConstraints gridBagConstraints28 = new GridBagConstraints();
			gridBagConstraints28.gridx = 0;
			gridBagConstraints28.fill = GridBagConstraints.BOTH;
			gridBagConstraints28.weightx = 0.1;
			gridBagConstraints28.weighty = 0.1;
			gridBagConstraints28.gridy = 0;
			pnlTranslatorMain = new JPanel();
			pnlTranslatorMain.setLayout(new GridBagLayout());
			pnlTranslatorMain.add(getPnlTranslatorSources(), gridBagConstraints28);
			pnlTranslatorMain.add(getPnlTranslatorTargets(), gridBagConstraints29);
			pnlTranslatorMain.add(getPnlTranslatorParts(), gridBagConstraints30);
			pnlTranslatorMain.add(getPnlTranslatorButtons(), gridBagConstraints31);
			pnlTranslatorMain.add(getPnlTranslatorDir(), gridBagConstraints37);
		}
		return pnlTranslatorMain;
	}

	/**
	 * This method initializes pnlTranslatorSources	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getPnlTranslatorSources() {
		if (pnlTranslatorSources == null) {
			GridBagConstraints gridBagConstraints34 = new GridBagConstraints();
			gridBagConstraints34.fill = GridBagConstraints.BOTH;
			gridBagConstraints34.gridy = 0;
			gridBagConstraints34.weightx = 1.0;
			gridBagConstraints34.weighty = 1.0;
			gridBagConstraints34.gridx = 0;
			pnlTranslatorSources = new JPanel();
			pnlTranslatorSources.setLayout(new GridBagLayout());
			pnlTranslatorSources.setBorder(BorderFactory.createTitledBorder(null, "Sources", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, new Font("Dialog", Font.BOLD, 12), new Color(51, 51, 51)));
			pnlTranslatorSources.add(getSpTranslatorSources(), gridBagConstraints34);
		}
		return pnlTranslatorSources;
	}

	/**
	 * This method initializes pnlTranslatorTargets	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getPnlTranslatorTargets() {
		if (pnlTranslatorTargets == null) {
			GridBagConstraints gridBagConstraints35 = new GridBagConstraints();
			gridBagConstraints35.fill = GridBagConstraints.BOTH;
			gridBagConstraints35.gridy = 0;
			gridBagConstraints35.weightx = 1.0;
			gridBagConstraints35.weighty = 1.0;
			gridBagConstraints35.gridx = 0;
			pnlTranslatorTargets = new JPanel();
			pnlTranslatorTargets.setLayout(new GridBagLayout());
			pnlTranslatorTargets.setBorder(BorderFactory.createTitledBorder(null, "Targets", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, new Font("Dialog", Font.BOLD, 12), new Color(51, 51, 51)));
			pnlTranslatorTargets.add(getSpTranslatorTargets(), gridBagConstraints35);
		}
		return pnlTranslatorTargets;
	}

	/**
	 * This method initializes pnlTranslatorParts	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getPnlTranslatorParts() {
		if (pnlTranslatorParts == null) {
			GridBagConstraints gridBagConstraints36 = new GridBagConstraints();
			gridBagConstraints36.fill = GridBagConstraints.BOTH;
			gridBagConstraints36.gridy = 0;
			gridBagConstraints36.weightx = 1.0;
			gridBagConstraints36.weighty = 1.0;
			gridBagConstraints36.gridx = 0;
			pnlTranslatorParts = new JPanel();
			pnlTranslatorParts.setLayout(new GridBagLayout());
			pnlTranslatorParts.setBorder(BorderFactory.createTitledBorder(null, "Parts", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, new Font("Dialog", Font.BOLD, 12), new Color(51, 51, 51)));
			pnlTranslatorParts.add(getSpTranslatorParts(), gridBagConstraints36);
		}
		return pnlTranslatorParts;
	}

	/**
	 * This method initializes pnlTranslatorButtons	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getPnlTranslatorButtons() {
		if (pnlTranslatorButtons == null) {
			GridBagConstraints gridBagConstraints33 = new GridBagConstraints();
			gridBagConstraints33.anchor = GridBagConstraints.EAST;
			gridBagConstraints33.fill = GridBagConstraints.NONE;
			GridBagConstraints gridBagConstraints32 = new GridBagConstraints();
			gridBagConstraints32.anchor = GridBagConstraints.EAST;
			gridBagConstraints32.fill = GridBagConstraints.NONE;
			pnlTranslatorButtons = new JPanel();
			pnlTranslatorButtons.setLayout(new GridBagLayout());
			pnlTranslatorButtons.add(getBtnTranslatorCancel(), gridBagConstraints33);
			pnlTranslatorButtons.add(getBtnTranslatorOk(), gridBagConstraints32);
		}
		return pnlTranslatorButtons;
	}

	/**
	 * This method initializes btnTranslatorCancel	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getBtnTranslatorCancel() {
		if (btnTranslatorCancel == null) {
			btnTranslatorCancel = new JButton();
			btnTranslatorCancel.setText("Cancel");
			btnTranslatorCancel.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					parent.handleEvents(EVT_BTN_TRANSLATOR_CANCEL_PRESSED);
				}
			});
		}
		return btnTranslatorCancel;
	}

	/**
	 * This method initializes btnTranslatorOk	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getBtnTranslatorOk() {
		if (btnTranslatorOk == null) {
			btnTranslatorOk = new JButton();
			btnTranslatorOk.setText("Ok");
			btnTranslatorOk.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					parent.handleEvents(EVT_BTN_TRANSLATOR_OK_PRESSED);
				}
			});
		}
		return btnTranslatorOk;
	}

	/**
	 * This method initializes spTranslatorSources	
	 * 	
	 * @return javax.swing.JScrollPane	
	 */
	private JScrollPane getSpTranslatorSources() {
		if (spTranslatorSources == null) {
			spTranslatorSources = new JScrollPane();
			spTranslatorSources.setViewportView(getLstTranslatorSources());
		}
		return spTranslatorSources;
	}

	/**
	 * This method initializes spTranslatorTargets	
	 * 	
	 * @return javax.swing.JScrollPane	
	 */
	private JScrollPane getSpTranslatorTargets() {
		if (spTranslatorTargets == null) {
			spTranslatorTargets = new JScrollPane();
			spTranslatorTargets.setViewportView(getLstTranslatorTargets());
		}
		return spTranslatorTargets;
	}

	/**
	 * This method initializes spTranslatorParts	
	 * 	
	 * @return javax.swing.JScrollPane	
	 */
	private JScrollPane getSpTranslatorParts() {
		if (spTranslatorParts == null) {
			spTranslatorParts = new JScrollPane();
			spTranslatorParts.setViewportView(getLstTranslatorParts());
		}
		return spTranslatorParts;
	}

	/**
	 * This method initializes lstTranslatorSources	
	 * 	
	 * @return javax.swing.JList	
	 */
	protected JList getLstTranslatorSources() {
		if (lstTranslatorSources == null) {
			lstTranslatorSources = new JList();
		}
		return lstTranslatorSources;
	}

	/**
	 * This method initializes lstTranslatorTargets	
	 * 	
	 * @return javax.swing.JList	
	 */
	protected JList getLstTranslatorTargets() {
		if (lstTranslatorTargets == null) {
			lstTranslatorTargets = new JList();
			lstTranslatorTargets.setSelectionMode(ListSelectionModel.MULTIPLE_INTERVAL_SELECTION);
		}
		return lstTranslatorTargets;
	}

	/**
	 * This method initializes lstTranslatorParts	
	 * 	
	 * @return javax.swing.JList	
	 */
	protected JList getLstTranslatorParts() {
		if (lstTranslatorParts == null) {
			lstTranslatorParts = new JList();
		}
		return lstTranslatorParts;
	}

	/**
	 * This method initializes pnlTranslatorDir	
	 * 	
	 * @return javax.swing.JPanel	
	 */
	private JPanel getPnlTranslatorDir() {
		if (pnlTranslatorDir == null) {
			GridBagConstraints gridBagConstraints39 = new GridBagConstraints();
			gridBagConstraints39.anchor = GridBagConstraints.EAST;
			GridBagConstraints gridBagConstraints38 = new GridBagConstraints();
			gridBagConstraints38.fill = GridBagConstraints.BOTH;
			gridBagConstraints38.weightx = 1.0;
			pnlTranslatorDir = new JPanel();
			pnlTranslatorDir.setLayout(new GridBagLayout());
			pnlTranslatorDir.setBorder(BorderFactory.createTitledBorder(null, "Directory", TitledBorder.DEFAULT_JUSTIFICATION, TitledBorder.DEFAULT_POSITION, new Font("Dialog", Font.BOLD, 12), new Color(51, 51, 51)));
			pnlTranslatorDir.add(getTfTranslatorFile(), gridBagConstraints38);
			pnlTranslatorDir.add(getBtnTranslatorChoose(), gridBagConstraints39);
		}
		return pnlTranslatorDir;
	}

	/**
	 * This method initializes tfTranslatorFile	
	 * 	
	 * @return javax.swing.JTextField	
	 */
	protected JTextField getTfTranslatorFile() {
		if (tfTranslatorFile == null) {
			tfTranslatorFile = new JTextField();
		}
		return tfTranslatorFile;
	}

	/**
	 * This method initializes btnTranslatorChoose	
	 * 	
	 * @return javax.swing.JButton	
	 */
	private JButton getBtnTranslatorChoose() {
		if (btnTranslatorChoose == null) {
			btnTranslatorChoose = new JButton();
			btnTranslatorChoose.setText("Choose");
			btnTranslatorChoose.addActionListener(new java.awt.event.ActionListener() {
				public void actionPerformed(java.awt.event.ActionEvent e) {
					parent.handleEvents(EVT_BTN_TRANSLATOR_CHOOSE_PRESSED);
				}
			});
		}
		return btnTranslatorChoose;
	}

}
