#include "MainWindow.h"
#include "dummy.h"
#include "FRR/Viewer1.h"
#include "FRR/ssao_viewer.h"
#include "FRR/separable.h"
#include "SRGGE/ex1.h"
#include "SRGGE/ex2.h"
#include "SRGGE/ex3.h"
#include "SRGGE/ex4.h"
#include "SRGGE/ex5.h"
#include "SRGGE/ex6.h"


QString frameStyle   = "QFrame#frame {				\
			border: 2px solid gray;			\
			border-radius: 9px;			\
			background-color: rgb(230, 232, 232);	\
			}";

MainWindow *MainWindowP = NULL;


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::MainWindow:
//
// MainWindow constructor.
//
MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
    setWindowTitle("OpenGL VIEWER");

	// set global variable for main window pointer
	MainWindowP = this;

	// create stacked widgets to hold multiple control panels and image displays
	m_stackWidgetPanels= new QStackedWidget;
	m_stackWidgetViews = new QStackedWidget;

	// create widgets, actions, and menus
	createWidgets();	// insert your widgets here
	createActions();	// insert your actions here
	createMenus();		// insert your menus   here

	// add stacked widget to vertical box layout 
	QFrame *frame = new QFrame;
	frame->setObjectName("frame");
	frame->setStyleSheet(frameStyle);
	frame->setMinimumWidth(300);
	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addWidget(m_stackWidgetPanels);
	vbox->addStretch(1);
	vbox->addLayout(createExitButtons());
	frame->setLayout(vbox);

	// add all widgets to grid layout
	QHBoxLayout *hbox = new QHBoxLayout;
	hbox->addWidget(createGroupView());
	hbox->setStretch(0, 1);
	hbox->addWidget(frame);

	// create container widget and set its layout
	QWidget *w = new QWidget;
	w->setLayout(hbox);
	setCentralWidget(w);

	// set stacked widget to latest solution
	m_stackWidgetViews->setCurrentIndex(DUMMY);
	m_stackWidgetPanels->setCurrentIndex(DUMMY);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::createWidgets:
//
// Create user-defined widgets for display and control panel.
//
void
MainWindow::createWidgets()
{
	// create list of hw names; m_hwName name will be used for
	// menu name and as key for class in m_hw container
	m_hwName<< "Blank page"
        << "SSAO"
        << "Separable Version"
        << "Exercise 1"
        << "Exercise 2"
        << "Excersise 3"
        << "Excersise 4"
        << "Excersise 5"
        << "Excersise 6";

	// format for legacy OpenGL with older GLSL (supporting attribute/varying qualifiers)
	QGLFormat glfLegacy = QGLFormat::defaultFormat();	// base format
	glfLegacy.setProfile(QGLFormat::CompatibilityProfile);	// also support legacy version
	glfLegacy.setSampleBuffers(true);			// multisample buffer support for antialiasing (AA)
	glfLegacy.setSamples(4);				// number of samples per fragment for AA
	glfLegacy.setDefaultFormat(glfLegacy);			// use modified parameters

	// format for modern OpenGL (3.3+) with newer GLSL (supporting in/out/layout)
	QGLFormat glfModern = QGLFormat::defaultFormat();	// base format
	glfModern.setVersion(3, 3);				// Mac requires 3.3+ for core profile
	glfModern.setProfile(QGLFormat::CoreProfile);		// don't use deprecated functions
	glfModern.setSampleBuffers(true);			// multisample buffer support for antialiasing (AA)
	glfModern.setSamples(4);				// number of samples per fragment (for AA)
	glfModern.setSwapInterval(0);
	glfModern.setDefaultFormat(glfModern);			// use modified parameters

	// instantiate homework solution classes
    m_hw[m_hwName[DUMMY]] = new Dummy(glfLegacy);
    m_hw[m_hwName[FRR1 ]] = new ssao_viewer(glfModern);
    m_hw[m_hwName[FRR2 ]] = new separable(glfModern);
    m_hw[m_hwName[SRGGE1 ]] = new ex1(glfModern);
    m_hw[m_hwName[SRGGE2 ]] = new ex2(glfModern);
    m_hw[m_hwName[SRGGE3 ]] = new ex3(glfModern);
    m_hw[m_hwName[SRGGE4 ]] = new ex4(glfModern);
    m_hw[m_hwName[SRGGE5 ]] = new ex5(glfModern);
    m_hw[m_hwName[SRGGE6 ]] = new ex6(glfModern);




	// add control panels to stacked widget
	for(int i = 0; i < (int) m_hwName.size(); ++i)
		m_stackWidgetPanels->addWidget(m_hw[m_hwName[i]]->controlPanel());
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// MainWindow::createActions:
//
// Create actions to associate with menu and toolbar selection.
//
void
MainWindow::createActions() 
{
    m_actionFRR1 = new QAction(m_hwName[FRR1], this);
    m_actionFRR1->setData(FRR1);

    m_actionFRR2 = new QAction(m_hwName[FRR2], this);
    m_actionFRR2->setData(FRR2);

    m_actionSRGGE1 = new QAction(m_hwName[SRGGE1], this);
    m_actionSRGGE1->setData(SRGGE1);

    m_actionSRGGE2 = new QAction(m_hwName[SRGGE2], this);
    m_actionSRGGE2->setData(SRGGE2);

    m_actionSRGGE3 = new QAction(m_hwName[SRGGE3], this);
    m_actionSRGGE3->setData(SRGGE3);

    m_actionSRGGE4 = new QAction(m_hwName[SRGGE4], this);
    m_actionSRGGE4->setData(SRGGE4);

    m_actionSRGGE5 = new QAction(m_hwName[SRGGE5], this);
    m_actionSRGGE5->setData(SRGGE5);

    m_actionSRGGE6 = new QAction(m_hwName[SRGGE6], this);
    m_actionSRGGE6->setData(SRGGE6);

	connect(menuBar(), SIGNAL(triggered(QAction*)), this, SLOT(changeHW(QAction*)));
}

void
MainWindow::createMenus() 
{
    m_menuFRR = menuBar()->addMenu("FRR");
    m_menuFRR->addAction(m_actionFRR1);
    m_menuFRR->addAction(m_actionFRR2);

    m_menuSRGGE = menuBar()->addMenu("SRGGE");
    m_menuSRGGE->addAction(m_actionSRGGE1);
    m_menuSRGGE->addAction(m_actionSRGGE2);
    m_menuSRGGE->addAction(m_actionSRGGE3);
    m_menuSRGGE->addAction(m_actionSRGGE4);
    m_menuSRGGE->addAction(m_actionSRGGE5);
    m_menuSRGGE->addAction(m_actionSRGGE6);




}


QFrame*
MainWindow::createGroupView()
{
	QFrame *frame = new QFrame();
	frame->setObjectName("frame");
	frame->setStyleSheet(frameStyle);

	// create a stack widget to handle multiple displays
	// one view per homework problem
	for(int i = 0; i < (int) m_hwName.size(); ++i)
		m_stackWidgetViews->addWidget(m_hw[m_hwName[i]]);

	// assemble stacked widget in vertical layout
	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addWidget(m_stackWidgetViews);
	frame->setLayout(vbox);

	return frame;
}




void MainWindow::changeHW(QAction* action)
{
	// get code from action
	int index = action->data().toInt();
	m_stackWidgetViews ->setCurrentIndex(index);	// change OpenGL widget 
	m_stackWidgetPanels->setCurrentIndex(index);	// change control panel
}



QHBoxLayout * MainWindow::createExitButtons()
{
	// create pushbuttons
	QPushButton *buttonReset = new QPushButton("Reset");
	QPushButton *buttonQuit  = new QPushButton("Quit");

	// init signal/slot connections
	connect(buttonReset, SIGNAL(clicked()), this, SLOT(reset()));
	connect(buttonQuit , SIGNAL(clicked()), this, SLOT(quit ()));

	// assemble pushbuttons in horizontal layout
	QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(buttonReset);
	buttonLayout->addWidget(buttonQuit );

	return buttonLayout;
}



void MainWindow::reset()
{
	int index = m_stackWidgetViews->currentIndex();	// index to current OpenGL widget
	m_hw[ m_hwName[index] ]->reset();		// invoke respective reset function
}



void MainWindow::quit()
{
	// close the dialog window
	close();
}
