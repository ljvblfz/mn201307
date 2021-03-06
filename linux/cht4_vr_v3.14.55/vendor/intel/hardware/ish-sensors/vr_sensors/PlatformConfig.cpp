#include <iostream>
#include <cstdlib>
#include <cutils/properties.h>
#include "PlatformConfig.hpp"
#include "VirtualSensor.hpp"

#define	M_PI		3.14159265358979323846

PlatformConfig::PlatformConfig()
{
        xmlDocPtr doc;
        xmlNodePtr root;
        std::string file;
        std::string prop;
        char buf[PROPERTY_VALUE_MAX];
        int ret=-1;

        ret = property_get("ro.sensors.mapper", buf, NULL);
        if (ret <= 0) {
                ALOGW("Get sensors mapper property error! Use default config.");
                prop="default";
        }
        else {
                prop=buf;
        }
        file="/system/etc/sensor_hal_config_"+prop+".xml";

        doc = xmlReadFile(file.c_str(), NULL, XML_PARSE_NOBLANKS);
        if (doc==NULL) {
                ALOGE("XML Document not parsed successfully.\n");
                return;
        }

        root=xmlDocGetRootElement(doc);
        if (root==NULL) {
                ALOGE("Empty XML document\n");
                xmlFreeDoc(doc);
                return;
        }

        if (xmlStrcmp(root->name, (const xmlChar *)"sensor_hal_config")) {
                ALOGE("Wrong XML document, cannot find \"sensor_hal_config\" element!\n");
                xmlFreeDoc(doc);
                return;
        }

        initXML(root->xmlChildrenNode);

        xmlFreeDoc(doc);
}

bool PlatformConfig::initXML(xmlNodePtr node)
{
        xmlNodePtr p;
        xmlChar *attr = NULL;
        std::string type;
        std::string category;

        while (node != NULL) {
                if (xmlStrcmp(node->name, (const xmlChar *)"sensor")) {
                        ALOGW("%s line:%d name: %s", __FUNCTION__, __LINE__, reinterpret_cast<const char*>(node->name));
                        node = node->next;
                        continue;
                }

                attr = xmlGetProp(node, (const xmlChar*)"type");
                if (attr == NULL) {
                        ALOGW("%s line:%d Cannot get sensor type!", __FUNCTION__, __LINE__);
                        node = node->next;
                        continue;
                }
                type = reinterpret_cast<const char *>(attr);

                attr = xmlGetProp(node, (const xmlChar*)"category");
                if (attr == NULL) {
                        ALOGW("Cannot get sensor category!");
                        category = "linux_driver";
                }
                else
                        category = reinterpret_cast<const char *>(attr);

                p=node->xmlChildrenNode;
                count = devices.size();
                while (p != NULL) {
                        if ((!xmlStrcmp(p->name, (const xmlChar *)"platform_config")))
                                addPlatformData(p, type);
                        else if ((!xmlStrcmp(p->name, (const xmlChar *)"device")))
                                addSensorDevice(p, type, category);
                        p=p->next;
                }
                node=node->next;
        }

        return true;
}

bool PlatformConfig::addPlatformData(xmlNodePtr node, std::string type)
{
        xmlChar *str = NULL;
        xmlChar *attr = NULL;
        struct PlatformData mData;
        xmlNodePtr p = node->xmlChildrenNode;

        attr = xmlGetProp(node, reinterpret_cast<const xmlChar*>("driver_node_type"));
        if (attr != NULL) {
                std::string nodeType = reinterpret_cast<char*>(attr);
                mData.driverNodeType = getDriverNodeType(nodeType);
        }
        else
                mData.driverNodeType = INPUT_EVENT;

        while (p != NULL) {
                str = xmlNodeGetContent(p);
                if (str == NULL || (!xmlStrcmp(str, (const xmlChar *)"0")) || (!xmlStrcmp(str, (const xmlChar *)""))) {
                        p = p->next;
                        continue;
                }

                if ((!xmlStrcmp(p->name, (const xmlChar *)"name"))) {
                        mData.name = reinterpret_cast<char *>(str);
                }
                else if ((!xmlStrcmp(p->name, (const xmlChar *)"activate_node"))) {
                        mData.activateInterface = reinterpret_cast<char *>(str);
                }
                else if ((!xmlStrcmp(p->name, (const xmlChar *)"poll_node"))) {
                        mData.setDelayInterface = reinterpret_cast<char *>(str);
                }
                else if ((!xmlStrcmp(p->name, (const xmlChar *)"data_node"))) {
                        mData.dataInterface = reinterpret_cast<char *>(str);
                }
                else if ((!xmlStrcmp(p->name, (const xmlChar *)"calibration_file"))) {
                        mData.calibrationFile = reinterpret_cast<char *>(str);
                }
                else if ((!xmlStrcmp(p->name, (const xmlChar *)"calibration_function"))) {
                        mData.calibrationFunc = reinterpret_cast<char *>(str);
                }
                else if ((!xmlStrcmp(p->name, (const xmlChar *)"driver_calibration_node"))) {
                        mData.driverCalibrationInterface = reinterpret_cast<char *>(str);
                }
                else if ((!xmlStrcmp(p->name, (const xmlChar *)"driver_calibration_file"))) {
                        mData.driverCalibrationFile = reinterpret_cast<char *>(str);
                }
                else if ((!xmlStrcmp(p->name, (const xmlChar *)"driver_calibration_function"))) {
                        mData.driverCalibrationFunc = reinterpret_cast<char *>(str);
                }
                else if ((!xmlStrcmp(p->name, (const xmlChar *)"filter_length"))) {
                        int value = atoi(reinterpret_cast<char *>(str));
                        mData.filterLength = value == 0 ? false : true;
                }
                xmlFree(str);
                p = p->next;
        }

        configs.insert(std::map<int,struct PlatformData>::value_type(count, mData));

        return true;
}

bool PlatformConfig::addSensorDevice(xmlNodePtr node, std::string type, std::string category)
{
        xmlChar *str = NULL;
        xmlChar *attr = NULL;
        float value = 0;
        SensorDevice mSensor;
        int sensorType = 0;
        const char* sensorStringType;
        uint32_t flags;
        sensors_event_property_t eventProperty;
        xmlNodePtr p = node->xmlChildrenNode;

        if (p == NULL) {
                ALOGE("%s line: %d invalid xml node!",
                      __FUNCTION__, __LINE__);
                return false;
        }

        mSensor.setId(devices.size());

        mSensor.setHandle(mSensor.idToHandle(mSensor.getId()));
        sensorType = getType(type);
        if (sensorType == 0) {
                ALOGE("%s line: %d Unsupported sensor type: %d",
                      __FUNCTION__, __LINE__, sensorType);
                return false;
        }
        mSensor.setType(sensorType);
        sensorStringType = getStringType(sensorType);
        mSensor.setStringType(sensorStringType);
        flags = getFlags(sensorType);
        mSensor.setFlags(flags);
        eventProperty = getEventProperty(sensorType);
        mSensor.setEventProperty(eventProperty);
        mSensor.setCategory(getCategory(category));

        while (p != NULL) {
                if ((!xmlStrcmp(p->name, (const xmlChar *)"mapper"))) {
                        attr = xmlGetProp(p, (const xmlChar*)"axis_x");
                        if (attr) {
                                std::string axis = reinterpret_cast<char *>(attr);
                                mSensor.setMapper(0, getAxis(axis));
                                xmlFree(attr);
                        }
                        attr = xmlGetProp(p, (const xmlChar*)"axis_y");
                        if (attr) {
                                std::string axis = reinterpret_cast<char *>(attr);
                                mSensor.setMapper(1, getAxis(axis));
                                xmlFree(attr);
                        }
                        attr = xmlGetProp(p, (const xmlChar*)"axis_z");
                        if (attr) {
                                std::string axis = reinterpret_cast<char *>(attr);
                                mSensor.setMapper(2, getAxis(axis));
                                xmlFree(attr);
                        }
                        attr = xmlGetProp(p, (const xmlChar*)"axis_w");
                        if (attr) {
                                std::string axis = reinterpret_cast<char *>(attr);
                                mSensor.setMapper(3, getAxis(axis));
                                xmlFree(attr);
                        }
                        p = p->next;
                        continue;
                }
                else if ((!xmlStrcmp(p->name, (const xmlChar *)"scale"))) {
                        float unit_value = 1.0;
                        attr = xmlGetProp(p, (const xmlChar*)"unit");
                        if (attr != NULL) {
                                std::string unit = reinterpret_cast<char *>(attr);
                                unit_value = getUnit(unit);
                        }
                        attr = xmlGetProp(p, (const xmlChar*)"axis_x");
                        if (attr) {
                                mSensor.setScale(0, atof(reinterpret_cast<char *>(attr)) * unit_value);
                                xmlFree(attr);
                        }
                        attr = xmlGetProp(p, (const xmlChar*)"axis_y");
                        if (attr) {
                                mSensor.setScale(1, atof(reinterpret_cast<char *>(attr)) * unit_value);
                                xmlFree(attr);
                        }
                        attr = xmlGetProp(p, (const xmlChar*)"axis_z");
                        if (attr) {
                                mSensor.setScale(2, atof(reinterpret_cast<char *>(attr)) * unit_value);
                                xmlFree(attr);
                        }
                        attr = xmlGetProp(p, (const xmlChar*)"axis_w");
                        if (attr) {
                                mSensor.setScale(3, atof(reinterpret_cast<char *>(attr)) * unit_value);
                                xmlFree(attr);
                        }
                        p = p->next;
                        continue;
                }
                str = xmlNodeGetContent(p);
                if (str == NULL || (!xmlStrcmp(str, (const xmlChar *)"0")) || (!xmlStrcmp(str, (const xmlChar *)""))) {
                        p = p->next;
                        continue;
                }

                if ((!xmlStrcmp(p->name, (const xmlChar *)"name"))){
                        mSensor.setName(reinterpret_cast<const char *>(str));
                        if (xmlStrstr(str, reinterpret_cast<const xmlChar *>("Secondary")) != NULL)
                                mSensor.setSubname(SECONDARY);
                }
                else if ((!xmlStrcmp(p->name, (const xmlChar *)"vendor"))) {
                        mSensor.setVendor(reinterpret_cast<const char *>(str));
                }
                else if ((!xmlStrcmp(p->name, (const xmlChar *)"version"))) {
                        mSensor.setVersion(atoi(reinterpret_cast<char *>(str)));
                }
                else if ((!xmlStrcmp(p->name, (const xmlChar *)"maxRange"))) {
                        value = atof(reinterpret_cast<char *>(str));
                        attr = xmlGetProp(p, (const xmlChar*)"unit");
                        if (attr != NULL) {
                                std::string unit = reinterpret_cast<char *>(attr);
                                mSensor.setMaxRange(value * getUnit(unit));
                        }
                        else
                                mSensor.setMaxRange(value);
                }
                else if ((!xmlStrcmp(p->name, (const xmlChar *)"resolution"))) {
                        value = atof(reinterpret_cast<char *>(str));
                        attr = xmlGetProp(p, (const xmlChar*)"unit");
                        if (attr != NULL) {
                                std::string unit = reinterpret_cast<char *>(attr);
                                mSensor.setResolution(value * getUnit(unit));
                        }
                        else
                                mSensor.setResolution(value);
                }
                else if ((!xmlStrcmp(p->name, (const xmlChar *)"power"))) {
                        mSensor.setPower(atof(reinterpret_cast<char *>(str)));
                }
                else if ((!xmlStrcmp(p->name, (const xmlChar *)"minDelay"))) {
                        mSensor.setMinDelay(atoi(reinterpret_cast<char *>(str)));
                }
                else if ((!xmlStrcmp(p->name, (const xmlChar *)"fifoReservedEventCount"))) {
                        int fifoReservedEventCount = atoi(reinterpret_cast<char *>(str));
                        mSensor.setFifoReservedEventCount(fifoReservedEventCount > 0 ? fifoReservedEventCount : 0);
                }
                else if ((!xmlStrcmp(p->name, (const xmlChar *)"fifoMaxEventCount"))) {
                        int fifoMaxEventCount = atoi(reinterpret_cast<char *>(str));
                        mSensor.setFifoMaxEventCount(fifoMaxEventCount > 0 ? fifoMaxEventCount : 0);
                }
                else if ((!xmlStrcmp(p->name, (const xmlChar *)"requiredPermission"))) {
                        mSensor.setRequiredPermission(reinterpret_cast<const char *>(str));
                }
                else if ((!xmlStrcmp(p->name, (const xmlChar *)"maxDelay"))) {
                        mSensor.setMaxDelay(atoi(reinterpret_cast<char *>(str)));
                }
                else if ((!xmlStrcmp(p->name, (const xmlChar *)"wakeup"))) {
                        int wakeup = atoi(reinterpret_cast<char *>(str));
                        flags = mSensor.getFlags();
                        if (wakeup)
                                flags |= SENSOR_FLAG_WAKE_UP;
                        else
                                flags &= ~SENSOR_FLAG_WAKE_UP;
                        mSensor.setFlags(flags);
                }
                xmlFree(str);
                p = p->next;
        }

        if (sensorType == SENSOR_TYPE_HEART_RATE)
                mSensor.setRequiredPermission(SENSOR_PERMISSION_BODY_SENSORS);
        else if (mSensor.getRequiredPermission() == NULL)
                mSensor.setRequiredPermission("");

        switch (sensorType) {
        case SENSOR_TYPE_SIGNIFICANT_MOTION:
        case SENSOR_TYPE_WAKE_GESTURE:
        case SENSOR_TYPE_GLANCE_GESTURE:
        case SENSOR_TYPE_PICK_UP_GESTURE:
        case SENSOR_TYPE_TILT_DETECTOR:
                flags = mSensor.getFlags();
                flags |= SENSOR_FLAG_WAKE_UP;
                mSensor.setFlags(flags);
                break;
        default:
                break;
        }

        devices.push_back(mSensor);

        return true;
}

int PlatformConfig::getType(std::string type)
{
        if (type.compare(0, 5, "light")==0)
                return SENSOR_TYPE_LIGHT;
        else if (type.compare(0, 9, "proximity")==0)
                return SENSOR_TYPE_PROXIMITY;
        else if (type.compare(0, 13, "accelerometer")==0)
                return SENSOR_TYPE_ACCELEROMETER;
        else if (type.compare(0, 15, "rotation_matrix") == 0)
                return SENSOR_TYPE_ROTATION_MATRIX;
		else if (type.compare(0, 7, "acc_raw") == 0)
				return SENSOR_TYPE_ACC_RAW; 
		else if (type.compare(0, 8, "gyro_raw") == 0)
				return SENSOR_TYPE_GYRO_RAW; 
		else if (type.compare(0, 9, "comps_raw") == 0)
				return SENSOR_TYPE_COMPS_RAW; 
        else if (type.compare(0, 7, "compass")==0)
                return SENSOR_TYPE_MAGNETIC_FIELD;
        else if (type.compare(0, 9, "gyroscope")==0)
                return SENSOR_TYPE_GYROSCOPE;
        else if (type.compare(0, 8, "pressure")==0)
                return SENSOR_TYPE_PRESSURE;
        else if (type.compare(0, 11, "temperature")==0)
                return SENSOR_TYPE_TEMPERATURE;
        else if (type.compare(0, 19, "ambient_temperature")==0)
                return SENSOR_TYPE_AMBIENT_TEMPERATURE;
        else if (type.compare(0, 17, "relative_humidity")==0)
                return SENSOR_TYPE_RELATIVE_HUMIDITY;
        else if (type.compare(0, 7, "gravity")==0)
                return SENSOR_TYPE_GRAVITY;
        else if (type.compare(0, 19, "linear_acceleration")==0)
                return SENSOR_TYPE_LINEAR_ACCELERATION;
        else if (type.compare(0, 15, "rotation_vector")==0)
                return SENSOR_TYPE_ROTATION_VECTOR;
        else if (type.compare(0, 11, "orientation")==0)
                return SENSOR_TYPE_ORIENTATION;
        else if (type.compare(0,13,"step_detector")==0)
                return SENSOR_TYPE_STEP_DETECTOR;
        else if (type.compare(0,12,"step_counter")==0)
                return SENSOR_TYPE_STEP_COUNTER;
        else if (type.compare(0,18,"significant_motion")==0)
                return SENSOR_TYPE_SIGNIFICANT_MOTION;
        else if (type.compare(0, 13, "gesture_flick")==0)
                return SENSOR_TYPE_GESTURE_FLICK;
        else if (type.compare(0, 8, "terminal")==0)
                return SENSOR_TYPE_TERMINAL;
        else if (type.compare(0, 5, "shake")==0)
                return SENSOR_TYPE_SHAKE;
        else if (type.compare(0, 14, "simple_tapping")==0)
                return SENSOR_TYPE_SIMPLE_TAPPING;
        else if (type.compare(0, 11, "move_detect")==0)
                return SENSOR_TYPE_MOVE_DETECT;
        else if (type.compare(0, 9, "pedometer")==0)
                return SENSOR_TYPE_PEDOMETER;
        else if (type.compare(0, 17, "physical_activity")==0)
                return SENSOR_TYPE_PHYSICAL_ACTIVITY;
        else if (type.compare(0, 16, "audio_classifier")==0)
                return SENSOR_TYPE_AUDIO_CLASSIFICATION;
        else if (type.compare(0, 20, "game_rotation_vector") ==0)
                return SENSOR_TYPE_GAME_ROTATION_VECTOR;
        else if (type.compare(0, 28, "geo_magnetic_rotation_vector") ==0)
                return SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR;
        else if (type.compare(0, 11, "calibration") ==0)
                return SENSOR_TYPE_CALIBRATION;
        else if (type.compare(0, 27, "uncalibrated_magnetic_field") == 0)
                return SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED;
        else if (type.compare(0, 22, "uncalibrated_gyroscope") == 0)
                return SENSOR_TYPE_GYROSCOPE_UNCALIBRATED;
        else if (type.compare(0, 11, "gesture_hmm") ==0)
                return SENSOR_TYPE_GESTURE_HMM;
        else if (type.compare(0, 16, "gesture_eartouch") ==0)
                return SENSOR_TYPE_GESTURE_EARTOUCH;
        else if (type.compare(0, 7, "gesture")==0)
                return SENSOR_TYPE_GESTURE;
        else if (type.compare(0, 15, "device_position")==0)
                return SENSOR_TYPE_DEVICE_POSITION;
        else if (type.compare(0, 4, "lift")==0)
                return SENSOR_TYPE_LIFT;
        else if (type.compare(0, 8, "pan_zoom")==0)
                return SENSOR_TYPE_PAN_ZOOM;
        else if (type.compare(0, 10, "heart_rate") == 0)
                return SENSOR_TYPE_HEART_RATE;
        else if (type.compare(0, 13, "tilt_detector") == 0)
                return SENSOR_TYPE_TILT_DETECTOR;
        else if (type.compare(0, 12, "wake_gesture") == 0)
                return SENSOR_TYPE_WAKE_GESTURE;
        else if (type.compare(0, 14, "glance_gesture") == 0)
                return SENSOR_TYPE_GLANCE_GESTURE;
        else if (type.compare(0, 15, "pick_up_gesture") == 0)
                return SENSOR_TYPE_PICK_UP_GESTURE;
        ALOGW("%s: unsupported sensor: %s", __FUNCTION__, type.c_str());
        return 0;
}

const char* PlatformConfig::getStringType(int sensorType)
{
        switch (sensorType) {
        case SENSOR_TYPE_LIGHT:
                return SENSOR_STRING_TYPE_LIGHT;
        case SENSOR_TYPE_ROTATION_MATRIX:
                return SENSOR_STRING_TYPE_ROTATION_MATRIX;
		case SENSOR_TYPE_ACC_RAW:
				return SENSOR_STRING_TYPE_ACC_RAW;
		case SENSOR_TYPE_GYRO_RAW:
				return SENSOR_STRING_TYPE_GYRO_RAW;
		case SENSOR_TYPE_COMPS_RAW:
				return SENSOR_STRING_TYPE_COMPS_RAW;
        case SENSOR_TYPE_PROXIMITY:
                return SENSOR_STRING_TYPE_PROXIMITY;
        case SENSOR_TYPE_ACCELEROMETER:
                return SENSOR_STRING_TYPE_ACCELEROMETER;
        case SENSOR_TYPE_MAGNETIC_FIELD:
                return SENSOR_STRING_TYPE_MAGNETIC_FIELD;
        case SENSOR_TYPE_GYROSCOPE:
                return SENSOR_STRING_TYPE_GYROSCOPE;
        case SENSOR_TYPE_PRESSURE:
                return SENSOR_STRING_TYPE_PRESSURE;
        case SENSOR_TYPE_TEMPERATURE:
                return SENSOR_STRING_TYPE_TEMPERATURE;
        case SENSOR_TYPE_AMBIENT_TEMPERATURE:
                return SENSOR_STRING_TYPE_AMBIENT_TEMPERATURE;
        case SENSOR_TYPE_RELATIVE_HUMIDITY:
                return SENSOR_STRING_TYPE_RELATIVE_HUMIDITY;
        case SENSOR_TYPE_GRAVITY:
                return SENSOR_STRING_TYPE_GRAVITY;
        case SENSOR_TYPE_LINEAR_ACCELERATION:
                return SENSOR_STRING_TYPE_LINEAR_ACCELERATION;
        case SENSOR_TYPE_ROTATION_VECTOR:
                return SENSOR_STRING_TYPE_ROTATION_VECTOR;
        case SENSOR_TYPE_ORIENTATION:
                return SENSOR_STRING_TYPE_ORIENTATION;
        case SENSOR_TYPE_STEP_DETECTOR:
                return SENSOR_STRING_TYPE_STEP_DETECTOR;
        case SENSOR_TYPE_STEP_COUNTER:
                return SENSOR_STRING_TYPE_STEP_COUNTER;
        case SENSOR_TYPE_SIGNIFICANT_MOTION:
                return SENSOR_STRING_TYPE_SIGNIFICANT_MOTION;
        case SENSOR_TYPE_GESTURE_FLICK:
                return SENSOR_STRING_TYPE_GESTURE_FLICK;
        case SENSOR_TYPE_TERMINAL:
                return SENSOR_STRING_TYPE_TERMINAL;
        case SENSOR_TYPE_SHAKE:
                return SENSOR_STRING_TYPE_SHAKE;
        case SENSOR_TYPE_SIMPLE_TAPPING:
                return SENSOR_STRING_TYPE_SIMPLE_TAPPING;
        case SENSOR_TYPE_MOVE_DETECT:
                return SENSOR_STRING_TYPE_MOVE_DETECT;
        case SENSOR_TYPE_PEDOMETER:
                return SENSOR_STRING_TYPE_PEDOMETER;
        case SENSOR_TYPE_PHYSICAL_ACTIVITY:
                return SENSOR_STRING_TYPE_PHYSICAL_ACTIVITY;
        case SENSOR_TYPE_AUDIO_CLASSIFICATION:
                return SENSOR_STRING_TYPE_AUDIO_CLASSIFICATION;
        case SENSOR_TYPE_GAME_ROTATION_VECTOR:
                return SENSOR_STRING_TYPE_GAME_ROTATION_VECTOR;
        case SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR:
                return SENSOR_STRING_TYPE_GEOMAGNETIC_ROTATION_VECTOR;
        case SENSOR_TYPE_CALIBRATION:
                return SENSOR_STRING_TYPE_CALIBRATION;
        case SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED:
                return SENSOR_STRING_TYPE_MAGNETIC_FIELD_UNCALIBRATED;
        case SENSOR_TYPE_GYROSCOPE_UNCALIBRATED:
                return SENSOR_STRING_TYPE_GYROSCOPE_UNCALIBRATED;
        case SENSOR_TYPE_GESTURE_HMM:
                return SENSOR_STRING_TYPE_GESTURE_HMM;
        case SENSOR_TYPE_GESTURE_EARTOUCH:
                return SENSOR_STRING_TYPE_GESTURE_EARTOUCH;
        case SENSOR_TYPE_GESTURE:
                return SENSOR_STRING_TYPE_GESTURE;
        case SENSOR_TYPE_DEVICE_POSITION:
                return SENSOR_STRING_TYPE_DEVICE_POSITION;
        case SENSOR_TYPE_LIFT:
                return SENSOR_STRING_TYPE_LIFT;
        case SENSOR_TYPE_PAN_ZOOM:
                return SENSOR_STRING_TYPE_PAN_ZOOM;
        case SENSOR_TYPE_HEART_RATE:
                return SENSOR_STRING_TYPE_HEART_RATE;
        case SENSOR_TYPE_TILT_DETECTOR:
                return SENSOR_STRING_TYPE_TILT_DETECTOR;
        case SENSOR_TYPE_WAKE_GESTURE:
                return SENSOR_STRING_TYPE_WAKE_GESTURE;
        case SENSOR_TYPE_GLANCE_GESTURE:
                return SENSOR_STRING_TYPE_GLANCE_GESTURE;
        case SENSOR_TYPE_PICK_UP_GESTURE:
                return SENSOR_STRING_TYPE_PICK_UP_GESTURE;
        default:
                ALOGW("%s: unsupported sensor: %d", __FUNCTION__, sensorType);
                break;
        }
        return NULL;
}

uint32_t PlatformConfig::getFlags(int sensorType)
{
        switch (sensorType) {
        case SENSOR_TYPE_LIGHT:
        case SENSOR_TYPE_STEP_COUNTER:
        case SENSOR_TYPE_HEART_RATE:
        case SENSOR_TYPE_TERMINAL:
        case SENSOR_TYPE_MOVE_DETECT:
        case SENSOR_TYPE_DEVICE_POSITION:
        case SENSOR_TYPE_PEDOMETER:
                return SENSOR_FLAG_ON_CHANGE_MODE;
        case SENSOR_TYPE_PROXIMITY:
                return SENSOR_FLAG_ON_CHANGE_MODE | SENSOR_FLAG_WAKE_UP;
        case SENSOR_TYPE_ACCELEROMETER:
        case SENSOR_TYPE_MAGNETIC_FIELD:
        case SENSOR_TYPE_GYROSCOPE:
        case SENSOR_TYPE_ROTATION_MATRIX:
		case SENSOR_TYPE_ACC_RAW:
		case SENSOR_TYPE_GYRO_RAW:
		case SENSOR_TYPE_COMPS_RAW:
        case SENSOR_TYPE_PRESSURE:
        case SENSOR_TYPE_TEMPERATURE:
        case SENSOR_TYPE_AMBIENT_TEMPERATURE:
        case SENSOR_TYPE_RELATIVE_HUMIDITY:
        case SENSOR_TYPE_GRAVITY:
        case SENSOR_TYPE_LINEAR_ACCELERATION:
        case SENSOR_TYPE_ROTATION_VECTOR:
        case SENSOR_TYPE_ORIENTATION:
        case SENSOR_TYPE_GAME_ROTATION_VECTOR:
        case SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR:
        case SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED:
        case SENSOR_TYPE_GYROSCOPE_UNCALIBRATED:
        case SENSOR_TYPE_CALIBRATION:
        case SENSOR_TYPE_PAN_ZOOM:
        case SENSOR_TYPE_AUDIO_CLASSIFICATION:
                return SENSOR_FLAG_CONTINUOUS_MODE;
        case SENSOR_TYPE_STEP_DETECTOR:
        case SENSOR_TYPE_GESTURE_FLICK:
        case SENSOR_TYPE_SHAKE:
        case SENSOR_TYPE_SIMPLE_TAPPING:
        case SENSOR_TYPE_GESTURE:
        case SENSOR_TYPE_LIFT:
                return SENSOR_FLAG_SPECIAL_REPORTING_MODE;
        case SENSOR_TYPE_SIGNIFICANT_MOTION:
        case SENSOR_TYPE_WAKE_GESTURE:
        case SENSOR_TYPE_GLANCE_GESTURE:
        case SENSOR_TYPE_PICK_UP_GESTURE:
                return SENSOR_FLAG_ONE_SHOT_MODE | SENSOR_FLAG_WAKE_UP;
        case SENSOR_TYPE_TILT_DETECTOR:
        case SENSOR_TYPE_PHYSICAL_ACTIVITY:
        case SENSOR_TYPE_GESTURE_HMM:
        case SENSOR_TYPE_GESTURE_EARTOUCH:
                return SENSOR_FLAG_SPECIAL_REPORTING_MODE | SENSOR_FLAG_WAKE_UP;
        default:
                ALOGW("%s: unsupported sensor: %d", __FUNCTION__, sensorType);
                break;
        }
        return SENSOR_FLAG_CONTINUOUS_MODE;
}

sensor_category_t PlatformConfig::getCategory(std::string category)
{
        if (category.length() == 0 || category.compare(0, 12, "linux_driver") == 0)
                return LINUX_DRIVER;
        else if (category.compare(0, 12, "libsensorhub") == 0)
                return LIBSENSORHUB;
        ALOGW("%s: unsupported sensor category: %s, using default",
             __FUNCTION__, category.c_str());
        return LINUX_DRIVER;
}

sensors_event_property_t PlatformConfig::getEventProperty(int type)
{
        switch (type) {
        case SENSOR_TYPE_ACCELEROMETER:
        case SENSOR_TYPE_MAGNETIC_FIELD:
        case SENSOR_TYPE_ORIENTATION:
        case SENSOR_TYPE_GYROSCOPE:
        case SENSOR_TYPE_ROTATION_MATRIX:
        case SENSOR_TYPE_ACC_RAW:
        case SENSOR_TYPE_GYRO_RAW:
        case SENSOR_TYPE_COMPS_RAW:
                return VECTOR;
        case SENSOR_TYPE_LIGHT:
        case SENSOR_TYPE_PRESSURE:
        case SENSOR_TYPE_TEMPERATURE:
        case SENSOR_TYPE_PROXIMITY:
        case SENSOR_TYPE_RELATIVE_HUMIDITY:
        case SENSOR_TYPE_AMBIENT_TEMPERATURE:
                return SCALAR;
        default:
                return OTHER;
        }
        return OTHER;
}

sensor_driver_node_type PlatformConfig::getDriverNodeType(std::string nodeType)
{
        if (nodeType.length() == 0 || nodeType.compare(0, 11, "input_event") == 0)
                return INPUT_EVENT;
        else if (nodeType.compare(0, 4, "misc") == 0)
                return MISC;
        ALOGW("%s: unsupported sensor driver node type: %s, using default",
             __FUNCTION__, nodeType.c_str());
        return INPUT_EVENT;
}

int PlatformConfig::getAxis(std::string axis)
{
        if (axis.compare("X")==0)
                return AXIS_X;
        else if (axis.compare("Y")==0)
                return AXIS_Y;
        else if (axis.compare("Z")==0)
                return AXIS_Z;
        else if (axis.compare("W")==0)
                return AXIS_W;
        return 0;
}

float PlatformConfig::getUnit(std::string unitName)
{
        if (unitName.compare("GRAVITY_EARTH")==0)
                return GRAVITY_EARTH;
        else if (unitName.compare("M_PI")==0)
                return M_PI;

        return 1.0;
}

bool PlatformConfig::getPlatformData(int id, struct PlatformData &data)
{
        std::map<int,struct PlatformData>::iterator it=configs.find(id);
        if (it==configs.end()) {
                ALOGE("cannot find PlatformData. id: %d\n", id);
                return false;
        }
        data=it->second;
        return true;
}

bool PlatformConfig::getSensorDevice(int id, SensorDevice &device)
{
        if (id < devices.size()) {
                device = devices[id];
                return true;
        }
        return false;
}
