// Copyright 2020 The Chromium OS Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
use std::any::Any;
use std::error;
use std::fmt;
use std::io;
use std::num::ParseIntError;
use std::path::PathBuf;
use std::sync::PoisonError;
use std::time;

use remain::sorted;

pub type Result<T> = std::result::Result<T, Error>;

#[sorted]
#[derive(Debug)]
pub enum Error {
    AlsaCardError(cros_alsa::CardError),
    AlsaControlError(cros_alsa::ControlError),
    CalibrationTimeout,
    CrasClientFailed(libcras::Error),
    DeserializationFailed(String, serde_yaml::Error),
    FileIOFailed(PathBuf, io::Error),
    InternalSpeakerNotFound,
    InvalidDatastore,
    InvalidDSMParam,
    InvalidShutDownTime,
    InvalidTemperature(i32),
    LargeCalibrationDiff(i32, i32),
    MissingDSMParam,
    MutexPoisonError,
    NewPlayStreamFailed(libcras::BoxError),
    NextPlaybackBufferFailed(libcras::BoxError),
    PlaybackFailed(io::Error),
    SerdeError(PathBuf, serde_yaml::Error),
    StartPlaybackTimeout,
    SystemTimeError(time::SystemTimeError),
    UnsupportedSoundCard(String),
    VPDParseFailed(String, ParseIntError),
    WorkerPanics(Box<dyn Any + Send + 'static>),
    ZeroPlayerIsNotRunning,
    ZeroPlayerIsRunning,
}

impl From<cros_alsa::CardError> for Error {
    fn from(err: cros_alsa::CardError) -> Error {
        Error::AlsaCardError(err)
    }
}

impl From<cros_alsa::ControlError> for Error {
    fn from(err: cros_alsa::ControlError) -> Error {
        Error::AlsaControlError(err)
    }
}

impl<T> From<PoisonError<T>> for Error {
    fn from(_: PoisonError<T>) -> Error {
        Error::MutexPoisonError
    }
}

impl error::Error for Error {}

impl fmt::Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        use Error::*;
        match self {
            AlsaCardError(e) => write!(f, "{}", e),
            AlsaControlError(e) => write!(f, "{}", e),
            CalibrationTimeout => write!(f, "calibration is not finished in time"),
            CrasClientFailed(e) => write!(f, "failed to create cras client: {}", e),
            DeserializationFailed(file_path, e) => {
                write!(f, "failed to parse {}: {}", file_path, e)
            }
            FileIOFailed(file_path, e) => write!(f, "{:#?}: {}", file_path, e),
            InvalidShutDownTime => write!(f, "invalid shutdown time"),
            InternalSpeakerNotFound => write!(f, "internal speaker is not found in cras"),
            InvalidTemperature(temp) => write!(
                f,
                "invalid calibration temperature: {}, and there is no datastore",
                temp
            ),
            InvalidDatastore => write!(f, "invalid datastore format"),
            InvalidDSMParam => write!(f, "invalid dsm param from kcontrol"),
            LargeCalibrationDiff(rdc, temp) => write!(
                f,
                "calibration difference is too large, rdc: {}, temp: {}",
                rdc, temp
            ),
            MissingDSMParam => write!(f, "missing dsm_param.bin"),
            MutexPoisonError => write!(f, "mutex is poisoned"),
            NewPlayStreamFailed(e) => write!(f, "{}", e),
            NextPlaybackBufferFailed(e) => write!(f, "{}", e),
            PlaybackFailed(e) => write!(f, "{}", e),
            SerdeError(file_path, e) => write!(f, "{:?}: {}", file_path, e),
            StartPlaybackTimeout => write!(f, "playback is not started in time"),
            SystemTimeError(e) => write!(f, "{}", e),
            UnsupportedSoundCard(name) => write!(f, "unsupported sound card: {}", name),
            VPDParseFailed(file_path, e) => write!(f, "failed to parse vpd {}: {}", file_path, e),
            WorkerPanics(e) => write!(f, "run_play_zero_worker panics: {:#?}", e),
            ZeroPlayerIsNotRunning => write!(f, "zero player is not running"),
            ZeroPlayerIsRunning => write!(f, "zero player is running"),
        }
    }
}
