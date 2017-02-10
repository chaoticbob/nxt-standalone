// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

class Camera {
    public:
        Camera()
            : _azimuth(glm::radians(45.f)),
              _altitude(glm::radians(30.f)),
              _radius(10.f),
              _center(0, 0, 0),
              _dirty(true) {
            recalculate();
        }

        void rotate(float dAzimuth, float dAltitude) {
            _dirty = true;
            _azimuth = glm::mod(_azimuth + dAzimuth, glm::radians(360.f));
            _altitude = glm::clamp(_altitude + dAltitude, glm::radians(-89.f), glm::radians(89.f));
        }

        void pan(float dX, float dY) {
            recalculate();
            glm::vec3 vX = glm::normalize(glm::cross(-_eyeDir, glm::vec3(0, 1, 0)));
            glm::vec3 vY = glm::normalize(glm::cross(_eyeDir, vX));
            _center += vX * dX * _radius + vY * dY * _radius;
        }

        void zoom(float factor) {
            _dirty = true;
            _radius = _radius * glm::exp(-factor);
        }

        glm::mat4 view() {
            if (_dirty) {
                recalculate();
            }
            return _view;
        }
    private:
        void recalculate() {
            glm::vec4 eye4 = glm::vec4(1, 0, 0, 1);
            eye4 = glm::rotate(glm::mat4(), _altitude, glm::vec3(0, 0, 1)) * eye4;
            eye4 = glm::rotate(glm::mat4(), _azimuth, glm::vec3(0, 1, 0)) * eye4;
            _eyeDir = glm::vec3(eye4);

            _view = glm::lookAt(_center + _eyeDir * _radius, _center, glm::vec3(0, 1, 0));
            _dirty = false;
        }
        float _azimuth;
        float _altitude;
        float _radius;
        glm::vec3 _center;
        glm::vec3 _eyeDir;
        bool _dirty;
        glm::mat4 _view;
};
