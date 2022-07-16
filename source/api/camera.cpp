#include "camera.hpp"

Camera::Camera() {

}

// ref: https://arm-software.github.io/opengl-es-sdk-for-android/terrain.html
void Camera::frustum(glm::mat4& view_projection)
{
    // Frustum planes are in world space.
    glm::mat4 inv = glm::inverse(view_projection);
    // Get world-space coordinates for clip-space bounds.
    glm::vec4 lbn = inv * glm::vec4(-1, -1, -1, 1);
    glm::vec4 ltn = inv * glm::vec4(-1, 1, -1, 1);
    glm::vec4 lbf = inv * glm::vec4(-1, -1, 1, 1);
    glm::vec4 rbn = inv * glm::vec4(1, -1, -1, 1);
    glm::vec4 rtn = inv * glm::vec4(1, 1, -1, 1);
    glm::vec4 rbf = inv * glm::vec4(1, -1, 1, 1);
    glm::vec4 rtf = inv * glm::vec4(1, 1, 1, 1);
    // Divide by w.
    glm::vec3 lbn_pos = glm::vec3(lbn / lbn.w);
    glm::vec3 ltn_pos = glm::vec3(ltn / ltn.w);
    glm::vec3 lbf_pos = glm::vec3(lbf / lbf.w);
    glm::vec3 rbn_pos = glm::vec3(rbn / rbn.w);
    glm::vec3 rtn_pos = glm::vec3(rtn / rtn.w);
    glm::vec3 rbf_pos = glm::vec3(rbf / rbf.w);
    glm::vec3 rtf_pos = glm::vec3(rtf / rtf.w);
    // Get plane normals for all sides of frustum.
    glm::vec3 left_normal = glm::normalize(glm::cross(lbf_pos - lbn_pos, ltn_pos - lbn_pos));
    glm::vec3 right_normal = glm::normalize(glm::cross(rtn_pos - rbn_pos, rbf_pos - rbn_pos));
    glm::vec3 top_normal = glm::normalize(glm::cross(ltn_pos - rtn_pos, rtf_pos - rtn_pos));
    glm::vec3 bottom_normal = glm::normalize(glm::cross(rbf_pos - rbn_pos, lbn_pos - rbn_pos));
    glm::vec3 near_normal = glm::normalize(glm::cross(ltn_pos - lbn_pos, rbn_pos - lbn_pos));
    glm::vec3 far_normal = glm::normalize(glm::cross(rtf_pos - rbf_pos, lbf_pos - rbf_pos));
    // Plane equations compactly represent a plane in 3D space.
    // We want a way to compute the distance to the plane while preserving the sign to know which side we're on.
    // Let:
    //    O: an arbitrary point on the plane
    //    N: the normal vector for the plane, pointing in the direction
    //       we want to be "positive".
    //    X: Position we want to check.
    //
    // Distance D to the plane can now be expressed as a simple operation:
    // D = dot((X - O), N) = dot(X, N) - dot(O, N)
    //
    // We can reduce this to one dot product by assuming that X is four-dimensional (4th component = 1.0).
    // The normal can be extended to four dimensions as well:
    // X' = vec4(X, 1.0)
    // N' = vec4(N, -dot(O, N))
    //
    // The expression now reduces to: D = dot(X', N')
    planes[0] = glm::vec4(near_normal, -glm::dot(near_normal, lbn_pos));   // Near
    planes[1] = glm::vec4(far_normal, -glm::dot(far_normal, lbf_pos));    // Far
    planes[2] = glm::vec4(left_normal, -glm::dot(left_normal, lbn_pos));   // Left
    planes[3] = glm::vec4(right_normal, -glm::dot(right_normal, rbn_pos));  // Right
    planes[4] = glm::vec4(top_normal, -glm::dot(top_normal, ltn_pos));    // Top
    planes[5] = glm::vec4(bottom_normal, -glm::dot(bottom_normal, lbn_pos)); // Bottom
}

// ref: https://arm-software.github.io/opengl-es-sdk-for-android/terrain.html
bool Camera::intersectsAABB(glm::vec3 start, glm::vec3 end)
{
    // If all corners of an axis-aligned bounding box are on the "wrong side" (negative distance)
    // of at least one of the frustum planes, we can safely cull the mesh.
    glm::vec4 corners[8];

    corners[0] = glm::vec4(start.x, start.y, start.z, 1);
    corners[1] = glm::vec4(start.x, start.y, end.z, 1);
    corners[2] = glm::vec4(start.x, end.y, start.z, 1);
    corners[3] = glm::vec4(start.x, end.y, end.z, 1);
    corners[4] = glm::vec4(end.x, start.y, start.z, 1);
    corners[5] = glm::vec4(end.x, start.y, end.z, 1);
    corners[6] = glm::vec4(end.x, end.y, start.z, 1);
    corners[7] = glm::vec4(end.x, end.y, end.z, 1);

    //for (unsigned int c = 0; c < 8; c++)
    //{
    //    // Require 4-dimensional coordinates for plane equations.
    //    corners[c] = glm::vec4(aabb[c], 1.0f);
    //}
    for (unsigned int p = 0; p < 6; p++)
    {
        bool inside_plane = false;
        for (unsigned int c = 0; c < 8; c++)
        {
            // If dot product > 0, we're "inside" the frustum plane,
            // otherwise, outside.
            if (glm::dot(corners[c], planes[p]) > 0.0f)
            {
                inside_plane = true;
                break;
            }
        }
        if (!inside_plane)
            return false;
    }
    return true;
}