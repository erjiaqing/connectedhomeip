/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

// THIS FILE IS GENERATED BY ZAP

#ifndef CHIP_CLUSTERS_H
#define CHIP_CLUSTERS_H

#import <Foundation/Foundation.h>

@class CHIPDevice;

typedef void (^ResponseHandler)(NSError * _Nullable error, NSDictionary * _Nullable values);

NS_ASSUME_NONNULL_BEGIN

/**
 * CHIPCluster
 *    This is the base class for clusters.
 */
@interface CHIPCluster : NSObject

- (nullable instancetype)initWithDevice:(CHIPDevice *)device
                               endpoint:(uint8_t)endpoint
                                  queue:(dispatch_queue_t)queue NS_DESIGNATED_INITIALIZER;
- (instancetype)init NS_UNAVAILABLE;
+ (instancetype)new NS_UNAVAILABLE;

@end

/**
 * Cluster Application Basic
 *
 */
@interface CHIPApplicationBasic : CHIPCluster

- (void)readAttributeVendorName:(ResponseHandler)completionHandler;
- (void)readAttributeVendorId:(ResponseHandler)completionHandler;
- (void)readAttributeApplicationName:(ResponseHandler)completionHandler;
- (void)readAttributeProductId:(ResponseHandler)completionHandler;
- (void)readAttributeApplicationId:(ResponseHandler)completionHandler;
- (void)readAttributeCatalogVendorId:(ResponseHandler)completionHandler;
- (void)readAttributeApplicationSatus:(ResponseHandler)completionHandler;
- (void)readAttributeClusterRevision:(ResponseHandler)completionHandler;

@end

/**
 * Cluster Barrier Control
 *
 */
@interface CHIPBarrierControl : CHIPCluster

- (void)barrierControlGoToPercent:(uint8_t)percentOpen completionHandler:(ResponseHandler)completionHandler;
- (void)barrierControlStop:(ResponseHandler)completionHandler;

- (void)readAttributeBarrierMovingState:(ResponseHandler)completionHandler;
- (void)readAttributeBarrierSafetyStatus:(ResponseHandler)completionHandler;
- (void)readAttributeBarrierCapabilities:(ResponseHandler)completionHandler;
- (void)readAttributeBarrierPosition:(ResponseHandler)completionHandler;
- (void)readAttributeClusterRevision:(ResponseHandler)completionHandler;

@end

/**
 * Cluster Basic
 *
 */
@interface CHIPBasic : CHIPCluster

- (void)mfgSpecificPing:(ResponseHandler)completionHandler;
- (void)resetToFactoryDefaults:(ResponseHandler)completionHandler;

- (void)readAttributeZclVersion:(ResponseHandler)completionHandler;
- (void)readAttributePowerSource:(ResponseHandler)completionHandler;
- (void)readAttributeClusterRevision:(ResponseHandler)completionHandler;

@end

/**
 * Cluster Binding
 *
 */
@interface CHIPBinding : CHIPCluster

- (void)bind:(uint64_t)nodeId
              groupId:(uint16_t)groupId
           endpointId:(uint8_t)endpointId
            clusterId:(uint16_t)clusterId
    completionHandler:(ResponseHandler)completionHandler;
- (void)unbind:(uint64_t)nodeId
              groupId:(uint16_t)groupId
           endpointId:(uint8_t)endpointId
            clusterId:(uint16_t)clusterId
    completionHandler:(ResponseHandler)completionHandler;

- (void)readAttributeClusterRevision:(ResponseHandler)completionHandler;

@end

/**
 * Cluster Color Control
 *
 */
@interface CHIPColorControl : CHIPCluster

- (void)moveColor:(int16_t)rateX
                rateY:(int16_t)rateY
          optionsMask:(uint8_t)optionsMask
      optionsOverride:(uint8_t)optionsOverride
    completionHandler:(ResponseHandler)completionHandler;
- (void)moveColorTemperature:(uint8_t)moveMode
                        rate:(uint16_t)rate
     colorTemperatureMinimum:(uint16_t)colorTemperatureMinimum
     colorTemperatureMaximum:(uint16_t)colorTemperatureMaximum
                 optionsMask:(uint8_t)optionsMask
             optionsOverride:(uint8_t)optionsOverride
           completionHandler:(ResponseHandler)completionHandler;
- (void)moveHue:(uint8_t)moveMode
                 rate:(uint8_t)rate
          optionsMask:(uint8_t)optionsMask
      optionsOverride:(uint8_t)optionsOverride
    completionHandler:(ResponseHandler)completionHandler;
- (void)moveSaturation:(uint8_t)moveMode
                  rate:(uint8_t)rate
           optionsMask:(uint8_t)optionsMask
       optionsOverride:(uint8_t)optionsOverride
     completionHandler:(ResponseHandler)completionHandler;
- (void)moveToColor:(uint16_t)colorX
               colorY:(uint16_t)colorY
       transitionTime:(uint16_t)transitionTime
          optionsMask:(uint8_t)optionsMask
      optionsOverride:(uint8_t)optionsOverride
    completionHandler:(ResponseHandler)completionHandler;
- (void)moveToColorTemperature:(uint16_t)colorTemperature
                transitionTime:(uint16_t)transitionTime
                   optionsMask:(uint8_t)optionsMask
               optionsOverride:(uint8_t)optionsOverride
             completionHandler:(ResponseHandler)completionHandler;
- (void)moveToHue:(uint8_t)hue
            direction:(uint8_t)direction
       transitionTime:(uint16_t)transitionTime
          optionsMask:(uint8_t)optionsMask
      optionsOverride:(uint8_t)optionsOverride
    completionHandler:(ResponseHandler)completionHandler;
- (void)moveToHueAndSaturation:(uint8_t)hue
                    saturation:(uint8_t)saturation
                transitionTime:(uint16_t)transitionTime
                   optionsMask:(uint8_t)optionsMask
               optionsOverride:(uint8_t)optionsOverride
             completionHandler:(ResponseHandler)completionHandler;
- (void)moveToSaturation:(uint8_t)saturation
          transitionTime:(uint16_t)transitionTime
             optionsMask:(uint8_t)optionsMask
         optionsOverride:(uint8_t)optionsOverride
       completionHandler:(ResponseHandler)completionHandler;
- (void)stepColor:(int16_t)stepX
                stepY:(int16_t)stepY
       transitionTime:(uint16_t)transitionTime
          optionsMask:(uint8_t)optionsMask
      optionsOverride:(uint8_t)optionsOverride
    completionHandler:(ResponseHandler)completionHandler;
- (void)stepColorTemperature:(uint8_t)stepMode
                    stepSize:(uint16_t)stepSize
              transitionTime:(uint16_t)transitionTime
     colorTemperatureMinimum:(uint16_t)colorTemperatureMinimum
     colorTemperatureMaximum:(uint16_t)colorTemperatureMaximum
                 optionsMask:(uint8_t)optionsMask
             optionsOverride:(uint8_t)optionsOverride
           completionHandler:(ResponseHandler)completionHandler;
- (void)stepHue:(uint8_t)stepMode
             stepSize:(uint8_t)stepSize
       transitionTime:(uint8_t)transitionTime
          optionsMask:(uint8_t)optionsMask
      optionsOverride:(uint8_t)optionsOverride
    completionHandler:(ResponseHandler)completionHandler;
- (void)stepSaturation:(uint8_t)stepMode
              stepSize:(uint8_t)stepSize
        transitionTime:(uint8_t)transitionTime
           optionsMask:(uint8_t)optionsMask
       optionsOverride:(uint8_t)optionsOverride
     completionHandler:(ResponseHandler)completionHandler;
- (void)stopMoveStep:(uint8_t)optionsMask
      optionsOverride:(uint8_t)optionsOverride
    completionHandler:(ResponseHandler)completionHandler;

- (void)readAttributeCurrentHue:(ResponseHandler)completionHandler;
- (void)configureAttributeCurrentHue:(uint16_t)minInterval
                         maxInterval:(uint16_t)maxInterval
                              change:(uint8_t)change
                   completionHandler:(ResponseHandler)completionHandler;
- (void)reportAttributeCurrentHue:(ResponseHandler)reportHandler;
- (void)readAttributeCurrentSaturation:(ResponseHandler)completionHandler;
- (void)configureAttributeCurrentSaturation:(uint16_t)minInterval
                                maxInterval:(uint16_t)maxInterval
                                     change:(uint8_t)change
                          completionHandler:(ResponseHandler)completionHandler;
- (void)reportAttributeCurrentSaturation:(ResponseHandler)reportHandler;
- (void)readAttributeRemainingTime:(ResponseHandler)completionHandler;
- (void)readAttributeCurrentX:(ResponseHandler)completionHandler;
- (void)configureAttributeCurrentX:(uint16_t)minInterval
                       maxInterval:(uint16_t)maxInterval
                            change:(uint16_t)change
                 completionHandler:(ResponseHandler)completionHandler;
- (void)reportAttributeCurrentX:(ResponseHandler)reportHandler;
- (void)readAttributeCurrentY:(ResponseHandler)completionHandler;
- (void)configureAttributeCurrentY:(uint16_t)minInterval
                       maxInterval:(uint16_t)maxInterval
                            change:(uint16_t)change
                 completionHandler:(ResponseHandler)completionHandler;
- (void)reportAttributeCurrentY:(ResponseHandler)reportHandler;
- (void)readAttributeDriftCompensation:(ResponseHandler)completionHandler;
- (void)readAttributeCompensationText:(ResponseHandler)completionHandler;
- (void)readAttributeColorTemperature:(ResponseHandler)completionHandler;
- (void)configureAttributeColorTemperature:(uint16_t)minInterval
                               maxInterval:(uint16_t)maxInterval
                                    change:(uint16_t)change
                         completionHandler:(ResponseHandler)completionHandler;
- (void)reportAttributeColorTemperature:(ResponseHandler)reportHandler;
- (void)readAttributeColorMode:(ResponseHandler)completionHandler;
- (void)readAttributeColorControlOptions:(ResponseHandler)completionHandler;
- (void)writeAttributeColorControlOptions:(uint8_t)value completionHandler:(ResponseHandler)completionHandler;
- (void)readAttributeNumberOfPrimaries:(ResponseHandler)completionHandler;
- (void)readAttributePrimary1X:(ResponseHandler)completionHandler;
- (void)readAttributePrimary1Y:(ResponseHandler)completionHandler;
- (void)readAttributePrimary1Intensity:(ResponseHandler)completionHandler;
- (void)readAttributePrimary2X:(ResponseHandler)completionHandler;
- (void)readAttributePrimary2Y:(ResponseHandler)completionHandler;
- (void)readAttributePrimary2Intensity:(ResponseHandler)completionHandler;
- (void)readAttributePrimary3X:(ResponseHandler)completionHandler;
- (void)readAttributePrimary3Y:(ResponseHandler)completionHandler;
- (void)readAttributePrimary3Intensity:(ResponseHandler)completionHandler;
- (void)readAttributePrimary4X:(ResponseHandler)completionHandler;
- (void)readAttributePrimary4Y:(ResponseHandler)completionHandler;
- (void)readAttributePrimary4Intensity:(ResponseHandler)completionHandler;
- (void)readAttributePrimary5X:(ResponseHandler)completionHandler;
- (void)readAttributePrimary5Y:(ResponseHandler)completionHandler;
- (void)readAttributePrimary5Intensity:(ResponseHandler)completionHandler;
- (void)readAttributePrimary6X:(ResponseHandler)completionHandler;
- (void)readAttributePrimary6Y:(ResponseHandler)completionHandler;
- (void)readAttributePrimary6Intensity:(ResponseHandler)completionHandler;
- (void)readAttributeWhitePointX:(ResponseHandler)completionHandler;
- (void)writeAttributeWhitePointX:(uint16_t)value completionHandler:(ResponseHandler)completionHandler;
- (void)readAttributeWhitePointY:(ResponseHandler)completionHandler;
- (void)writeAttributeWhitePointY:(uint16_t)value completionHandler:(ResponseHandler)completionHandler;
- (void)readAttributeColorPointRX:(ResponseHandler)completionHandler;
- (void)writeAttributeColorPointRX:(uint16_t)value completionHandler:(ResponseHandler)completionHandler;
- (void)readAttributeColorPointRY:(ResponseHandler)completionHandler;
- (void)writeAttributeColorPointRY:(uint16_t)value completionHandler:(ResponseHandler)completionHandler;
- (void)readAttributeColorPointRIntensity:(ResponseHandler)completionHandler;
- (void)writeAttributeColorPointRIntensity:(uint8_t)value completionHandler:(ResponseHandler)completionHandler;
- (void)readAttributeColorPointGX:(ResponseHandler)completionHandler;
- (void)writeAttributeColorPointGX:(uint16_t)value completionHandler:(ResponseHandler)completionHandler;
- (void)readAttributeColorPointGY:(ResponseHandler)completionHandler;
- (void)writeAttributeColorPointGY:(uint16_t)value completionHandler:(ResponseHandler)completionHandler;
- (void)readAttributeColorPointGIntensity:(ResponseHandler)completionHandler;
- (void)writeAttributeColorPointGIntensity:(uint8_t)value completionHandler:(ResponseHandler)completionHandler;
- (void)readAttributeColorPointBX:(ResponseHandler)completionHandler;
- (void)writeAttributeColorPointBX:(uint16_t)value completionHandler:(ResponseHandler)completionHandler;
- (void)readAttributeColorPointBY:(ResponseHandler)completionHandler;
- (void)writeAttributeColorPointBY:(uint16_t)value completionHandler:(ResponseHandler)completionHandler;
- (void)readAttributeColorPointBIntensity:(ResponseHandler)completionHandler;
- (void)writeAttributeColorPointBIntensity:(uint8_t)value completionHandler:(ResponseHandler)completionHandler;
- (void)readAttributeEnhancedCurrentHue:(ResponseHandler)completionHandler;
- (void)readAttributeEnhancedColorMode:(ResponseHandler)completionHandler;
- (void)readAttributeColorLoopActive:(ResponseHandler)completionHandler;
- (void)readAttributeColorLoopDirection:(ResponseHandler)completionHandler;
- (void)readAttributeColorLoopTime:(ResponseHandler)completionHandler;
- (void)readAttributeColorCapabilities:(ResponseHandler)completionHandler;
- (void)readAttributeColorTempPhysicalMin:(ResponseHandler)completionHandler;
- (void)readAttributeColorTempPhysicalMax:(ResponseHandler)completionHandler;
- (void)readAttributeCoupleColorTempToLevelMinMireds:(ResponseHandler)completionHandler;
- (void)readAttributeStartUpColorTemperatureMireds:(ResponseHandler)completionHandler;
- (void)writeAttributeStartUpColorTemperatureMireds:(uint16_t)value completionHandler:(ResponseHandler)completionHandler;
- (void)readAttributeClusterRevision:(ResponseHandler)completionHandler;

@end

/**
 * Cluster Content Launch
 *
 */
@interface CHIPContentLaunch : CHIPCluster

- (void)launchContent:(ResponseHandler)completionHandler;
- (void)launchURL:(ResponseHandler)completionHandler;

- (void)readAttributeClusterRevision:(ResponseHandler)completionHandler;

@end

/**
 * Cluster Door Lock
 *
 */
@interface CHIPDoorLock : CHIPCluster

- (void)clearAllPins:(ResponseHandler)completionHandler;
- (void)clearAllRfids:(ResponseHandler)completionHandler;
- (void)clearHolidaySchedule:(uint8_t)scheduleId completionHandler:(ResponseHandler)completionHandler;
- (void)clearPin:(uint16_t)userId completionHandler:(ResponseHandler)completionHandler;
- (void)clearRfid:(uint16_t)userId completionHandler:(ResponseHandler)completionHandler;
- (void)clearWeekdaySchedule:(uint8_t)scheduleId userId:(uint16_t)userId completionHandler:(ResponseHandler)completionHandler;
- (void)clearYeardaySchedule:(uint8_t)scheduleId userId:(uint16_t)userId completionHandler:(ResponseHandler)completionHandler;
- (void)getHolidaySchedule:(uint8_t)scheduleId completionHandler:(ResponseHandler)completionHandler;
- (void)getLogRecord:(uint16_t)logIndex completionHandler:(ResponseHandler)completionHandler;
- (void)getPin:(uint16_t)userId completionHandler:(ResponseHandler)completionHandler;
- (void)getRfid:(uint16_t)userId completionHandler:(ResponseHandler)completionHandler;
- (void)getUserType:(uint16_t)userId completionHandler:(ResponseHandler)completionHandler;
- (void)getWeekdaySchedule:(uint8_t)scheduleId userId:(uint16_t)userId completionHandler:(ResponseHandler)completionHandler;
- (void)getYeardaySchedule:(uint8_t)scheduleId userId:(uint16_t)userId completionHandler:(ResponseHandler)completionHandler;
- (void)lockDoor:(char *)pin completionHandler:(ResponseHandler)completionHandler;
- (void)setHolidaySchedule:(uint8_t)scheduleId
                localStartTime:(uint32_t)localStartTime
                  localEndTime:(uint32_t)localEndTime
    operatingModeDuringHoliday:(uint8_t)operatingModeDuringHoliday
             completionHandler:(ResponseHandler)completionHandler;
- (void)setPin:(uint16_t)userId
           userStatus:(uint8_t)userStatus
             userType:(uint8_t)userType
                  pin:(char *)pin
    completionHandler:(ResponseHandler)completionHandler;
- (void)setRfid:(uint16_t)userId
           userStatus:(uint8_t)userStatus
             userType:(uint8_t)userType
                   id:(char *)id
    completionHandler:(ResponseHandler)completionHandler;
- (void)setUserType:(uint16_t)userId userType:(uint8_t)userType completionHandler:(ResponseHandler)completionHandler;
- (void)setWeekdaySchedule:(uint8_t)scheduleId
                    userId:(uint16_t)userId
                  daysMask:(uint8_t)daysMask
                 startHour:(uint8_t)startHour
               startMinute:(uint8_t)startMinute
                   endHour:(uint8_t)endHour
                 endMinute:(uint8_t)endMinute
         completionHandler:(ResponseHandler)completionHandler;
- (void)setYeardaySchedule:(uint8_t)scheduleId
                    userId:(uint16_t)userId
            localStartTime:(uint32_t)localStartTime
              localEndTime:(uint32_t)localEndTime
         completionHandler:(ResponseHandler)completionHandler;
- (void)unlockDoor:(char *)pin completionHandler:(ResponseHandler)completionHandler;
- (void)unlockWithTimeout:(uint16_t)timeoutInSeconds pin:(char *)pin completionHandler:(ResponseHandler)completionHandler;

- (void)readAttributeLockState:(ResponseHandler)completionHandler;
- (void)configureAttributeLockState:(uint16_t)minInterval
                        maxInterval:(uint16_t)maxInterval
                  completionHandler:(ResponseHandler)completionHandler;
- (void)reportAttributeLockState:(ResponseHandler)reportHandler;
- (void)readAttributeLockType:(ResponseHandler)completionHandler;
- (void)readAttributeActuatorEnabled:(ResponseHandler)completionHandler;
- (void)readAttributeClusterRevision:(ResponseHandler)completionHandler;

@end

/**
 * Cluster General Commissioning
 *
 */
@interface CHIPGeneralCommissioning : CHIPCluster

- (void)armFailSafe:(uint16_t)expiryLengthSeconds
           breadcrumb:(uint64_t)breadcrumb
            timeoutMs:(uint32_t)timeoutMs
    completionHandler:(ResponseHandler)completionHandler;
- (void)commissioningComplete:(ResponseHandler)completionHandler;
- (void)setFabric:(char *)fabricId
         fabricSecret:(char *)fabricSecret
           breadcrumb:(uint64_t)breadcrumb
            timeoutMs:(uint32_t)timeoutMs
    completionHandler:(ResponseHandler)completionHandler;

- (void)readAttributeFabricId:(ResponseHandler)completionHandler;
- (void)readAttributeBreadcrumb:(ResponseHandler)completionHandler;
- (void)writeAttributeBreadcrumb:(uint64_t)value completionHandler:(ResponseHandler)completionHandler;
- (void)readAttributeClusterRevision:(ResponseHandler)completionHandler;

@end

/**
 * Cluster Groups
 *
 */
@interface CHIPGroups : CHIPCluster

- (void)addGroup:(uint16_t)groupId groupName:(char *)groupName completionHandler:(ResponseHandler)completionHandler;
- (void)addGroupIfIdentifying:(uint16_t)groupId groupName:(char *)groupName completionHandler:(ResponseHandler)completionHandler;
- (void)getGroupMembership:(uint8_t)groupCount groupList:(uint16_t)groupList completionHandler:(ResponseHandler)completionHandler;
- (void)removeAllGroups:(ResponseHandler)completionHandler;
- (void)removeGroup:(uint16_t)groupId completionHandler:(ResponseHandler)completionHandler;
- (void)viewGroup:(uint16_t)groupId completionHandler:(ResponseHandler)completionHandler;

- (void)readAttributeNameSupport:(ResponseHandler)completionHandler;
- (void)readAttributeClusterRevision:(ResponseHandler)completionHandler;

@end

/**
 * Cluster Identify
 *
 */
@interface CHIPIdentify : CHIPCluster

- (void)identify:(uint16_t)identifyTime completionHandler:(ResponseHandler)completionHandler;
- (void)identifyQuery:(ResponseHandler)completionHandler;

- (void)readAttributeIdentifyTime:(ResponseHandler)completionHandler;
- (void)writeAttributeIdentifyTime:(uint16_t)value completionHandler:(ResponseHandler)completionHandler;
- (void)readAttributeClusterRevision:(ResponseHandler)completionHandler;

@end

/**
 * Cluster Level Control
 *
 */
@interface CHIPLevelControl : CHIPCluster

- (void)move:(uint8_t)moveMode
                 rate:(uint8_t)rate
           optionMask:(uint8_t)optionMask
       optionOverride:(uint8_t)optionOverride
    completionHandler:(ResponseHandler)completionHandler;
- (void)moveToLevel:(uint8_t)level
       transitionTime:(uint16_t)transitionTime
           optionMask:(uint8_t)optionMask
       optionOverride:(uint8_t)optionOverride
    completionHandler:(ResponseHandler)completionHandler;
- (void)moveToLevelWithOnOff:(uint8_t)level
              transitionTime:(uint16_t)transitionTime
           completionHandler:(ResponseHandler)completionHandler;
- (void)moveWithOnOff:(uint8_t)moveMode rate:(uint8_t)rate completionHandler:(ResponseHandler)completionHandler;
- (void)step:(uint8_t)stepMode
             stepSize:(uint8_t)stepSize
       transitionTime:(uint16_t)transitionTime
           optionMask:(uint8_t)optionMask
       optionOverride:(uint8_t)optionOverride
    completionHandler:(ResponseHandler)completionHandler;
- (void)stepWithOnOff:(uint8_t)stepMode
             stepSize:(uint8_t)stepSize
       transitionTime:(uint16_t)transitionTime
    completionHandler:(ResponseHandler)completionHandler;
- (void)stop:(uint8_t)optionMask optionOverride:(uint8_t)optionOverride completionHandler:(ResponseHandler)completionHandler;
- (void)stopWithOnOff:(ResponseHandler)completionHandler;

- (void)readAttributeCurrentLevel:(ResponseHandler)completionHandler;
- (void)configureAttributeCurrentLevel:(uint16_t)minInterval
                           maxInterval:(uint16_t)maxInterval
                                change:(uint8_t)change
                     completionHandler:(ResponseHandler)completionHandler;
- (void)reportAttributeCurrentLevel:(ResponseHandler)reportHandler;
- (void)readAttributeClusterRevision:(ResponseHandler)completionHandler;

@end

/**
 * Cluster Media Playback
 *
 */
@interface CHIPMediaPlayback : CHIPCluster

- (void)fastForwardRequest:(ResponseHandler)completionHandler;
- (void)nextRequest:(ResponseHandler)completionHandler;
- (void)pauseRequest:(ResponseHandler)completionHandler;
- (void)playRequest:(ResponseHandler)completionHandler;
- (void)previousRequest:(ResponseHandler)completionHandler;
- (void)rewindRequest:(ResponseHandler)completionHandler;
- (void)skipBackwardRequest:(ResponseHandler)completionHandler;
- (void)skipForwardRequest:(ResponseHandler)completionHandler;
- (void)startOverRequest:(ResponseHandler)completionHandler;
- (void)stopRequest:(ResponseHandler)completionHandler;

- (void)readAttributeCurrentState:(ResponseHandler)completionHandler;
- (void)readAttributeClusterRevision:(ResponseHandler)completionHandler;

@end

/**
 * Cluster On/off
 *
 */
@interface CHIPOnOff : CHIPCluster

- (void)off:(ResponseHandler)completionHandler;
- (void)on:(ResponseHandler)completionHandler;
- (void)toggle:(ResponseHandler)completionHandler;

- (void)readAttributeOnOff:(ResponseHandler)completionHandler;
- (void)configureAttributeOnOff:(uint16_t)minInterval
                    maxInterval:(uint16_t)maxInterval
              completionHandler:(ResponseHandler)completionHandler;
- (void)reportAttributeOnOff:(ResponseHandler)reportHandler;
- (void)readAttributeClusterRevision:(ResponseHandler)completionHandler;

@end

/**
 * Cluster Scenes
 *
 */
@interface CHIPScenes : CHIPCluster

- (void)addScene:(uint16_t)groupId
              sceneId:(uint8_t)sceneId
       transitionTime:(uint16_t)transitionTime
            sceneName:(char *)sceneName
            clusterId:(uint16_t)clusterId
               length:(uint8_t)length
                value:(uint8_t)value
    completionHandler:(ResponseHandler)completionHandler;
- (void)getSceneMembership:(uint16_t)groupId completionHandler:(ResponseHandler)completionHandler;
- (void)recallScene:(uint16_t)groupId
              sceneId:(uint8_t)sceneId
       transitionTime:(uint16_t)transitionTime
    completionHandler:(ResponseHandler)completionHandler;
- (void)removeAllScenes:(uint16_t)groupId completionHandler:(ResponseHandler)completionHandler;
- (void)removeScene:(uint16_t)groupId sceneId:(uint8_t)sceneId completionHandler:(ResponseHandler)completionHandler;
- (void)storeScene:(uint16_t)groupId sceneId:(uint8_t)sceneId completionHandler:(ResponseHandler)completionHandler;
- (void)viewScene:(uint16_t)groupId sceneId:(uint8_t)sceneId completionHandler:(ResponseHandler)completionHandler;

- (void)readAttributeSceneCount:(ResponseHandler)completionHandler;
- (void)readAttributeCurrentScene:(ResponseHandler)completionHandler;
- (void)readAttributeCurrentGroup:(ResponseHandler)completionHandler;
- (void)readAttributeSceneValid:(ResponseHandler)completionHandler;
- (void)readAttributeNameSupport:(ResponseHandler)completionHandler;
- (void)readAttributeClusterRevision:(ResponseHandler)completionHandler;

@end

/**
 * Cluster Temperature Measurement
 *
 */
@interface CHIPTemperatureMeasurement : CHIPCluster

- (void)readAttributeMeasuredValue:(ResponseHandler)completionHandler;
- (void)configureAttributeMeasuredValue:(uint16_t)minInterval
                            maxInterval:(uint16_t)maxInterval
                                 change:(int16_t)change
                      completionHandler:(ResponseHandler)completionHandler;
- (void)reportAttributeMeasuredValue:(ResponseHandler)reportHandler;
- (void)readAttributeMinMeasuredValue:(ResponseHandler)completionHandler;
- (void)readAttributeMaxMeasuredValue:(ResponseHandler)completionHandler;
- (void)readAttributeClusterRevision:(ResponseHandler)completionHandler;

@end

NS_ASSUME_NONNULL_END

#endif /* CHIP_CLUSTERS_H */
