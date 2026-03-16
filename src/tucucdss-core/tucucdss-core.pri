
INCLUDEPATH += $$PWD/../../src \
               $$PWD/../../libs \

!config_withpdf {
    DEFINES += CONFIG_WITHPDF
}

config_withpdf {
# Setup wkhtmltopdf for windows.
win32 {
    LIBS += $$PWD/../../libs/wkhtmltox/windows/wkhtmltox.dll

    COPY_FROM_PATH=$$shell_path($$PWD/../../libs/wkhtmltox/windows/wkhtmltox.dll)
    COPY_TO_PATH=$$shell_path($$OUT_PWD)
} else {
    LIBS += $$PWD/../../libs/wkhtmltox/ubuntu/libwkhtmltox.so.0.12.6

    COPY_FROM_PATH=$$PWD/../../libs/wkhtmltox/ubuntu/libwkhtmltox.so.0.12.6
    COPY_TO_PATH=$$OUT_PWD
}

copydata.commands = $(COPY_DIR) $$COPY_FROM_PATH $$COPY_TO_PATH
first.depends = $(first) copydata

export(first.depends)
export(copydata.commands)
}

QMAKE_EXTRA_TARGETS += first copydata

HEADERS += \
    $$PWD/computer.h \
    $$PWD/exporter/abstracthtmlexport.h \
    $$PWD/exporter/abstractxpertrequestresultexport.h \
    $$PWD/exporter/static/filestring.h \
    $$PWD/exporter/xpertrequestresulthtmlexport.h \
    $$PWD/exporter/xpertrequestresultxmlexport.h \
    $$PWD/flow/abstract/abstractxpertflowstep.h \
    $$PWD/flow/abstract/abstractxpertflowstepprovider.h \
    $$PWD/flow/general/covariatevalidatorandmodelselector.h \
    $$PWD/flow/general/adjustmenttraitcreator.h \
    $$PWD/flow/general/datevalidator.h \
    $$PWD/flow/general/dosevalidator.h \
    $$PWD/flow/general/generalxpertflowstepprovider.h \
    $$PWD/flow/general/reportprinter.h \
    $$PWD/flow/general/requestexecutor.h \
    $$PWD/flow/general/samplevalidator.h \
    $$PWD/flow/general/targetvalidator.h \
    $$PWD/query/admindata.h \
    $$PWD/query/xpertquerydata.h \
    $$PWD/query/xpertqueryimport.h \
    $$PWD/query/xpertquerytocoreextractor.h \
    $$PWD/query/xpertrequestdata.h \
    $$PWD/language/languageexception.h \
    $$PWD/language/languagemanager.h \
    $$PWD/result/abstractvalidationresult.h \
    $$PWD/result/covariatevalidationresult.h \
    $$PWD/result/dosevalidationresult.h \
    $$PWD/result/dosedatevalidationresult.h \
    $$PWD/result/sampledatevalidationresult.h \
    $$PWD/result/samplevalidationresult.h \
    $$PWD/result/xpertqueryresult.h \
    $$PWD/result/xpertrequestresult.h \
    $$PWD/utils/xpertutils.h

SOURCES += \
    $$PWD/computer.cpp \
    $$PWD/exporter/static/filestring.cpp \
    $$PWD/exporter/xpertrequestresulthtmlexport.cpp \
    $$PWD/exporter/xpertrequestresultxmlexport.cpp \
    $$PWD/flow/general/covariatevalidatorandmodelselector.cpp \
    $$PWD/flow/general/adjustmenttraitcreator.cpp \
    $$PWD/flow/general/datevalidator.cpp \
    $$PWD/flow/general/dosevalidator.cpp \
    $$PWD/flow/general/generalxpertflowstepprovider.cpp \
    $$PWD/flow/general/reportprinter.cpp \
    $$PWD/flow/general/requestexecutor.cpp \
    $$PWD/flow/general/samplevalidator.cpp \
    $$PWD/flow/general/targetvalidator.cpp \
    $$PWD/query/admindata.cpp \
    $$PWD/query/xpertquerydata.cpp \
    $$PWD/query/xpertqueryimport.cpp \
    $$PWD/language/languageexception.cpp \
    $$PWD/language/languagemanager.cpp \
    $$PWD/query/xpertquerytocoreextractor.cpp \
    $$PWD/query/xpertrequestdata.cpp \
    $$PWD/result/covariatevalidationresult.cpp \
    $$PWD/result/dosedatevalidationresult.cpp \
    $$PWD/result/sampledatevalidationresult.cpp \
    $$PWD/result/dosevalidationresult.cpp \
    $$PWD/result/samplevalidationresult.cpp \
    $$PWD/result/xpertqueryresult.cpp \
    $$PWD/result/xpertrequestresult.cpp \
    $$PWD/utils/xpertutils.cpp

config_withpdf {
HEADERS += $$PWD/exporter/xpertrequestresultpdfexport.cpp
SOURCES += $$PWD/exporter/xpertrequestresultpdfexport.h
}
