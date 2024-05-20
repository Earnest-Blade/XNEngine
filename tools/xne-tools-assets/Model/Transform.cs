using System.Numerics;
using Assimp;

using Matrix4x4 = Assimp.Matrix4x4;
using Quaternion = Assimp.Quaternion;

namespace xne.tools.assets
{
    public struct Transform
    {
        public Vector3 Position;
        public Vector4 Rotation;
        public Vector3 Scale;

        public Transform(Matrix4x4 mat)
        {
            Position = new Vector3(mat[3, 0], mat[3, 1], mat[3, 2]);

            Scale = new Vector3
            {
                X = MathF.Sqrt(new Vector3(mat[0, 0], mat[0, 1], mat[0, 2]).LengthSquared()),
                Y = MathF.Sqrt(new Vector3(mat[1, 0], mat[1, 1], mat[1, 2]).LengthSquared()),
                Z = MathF.Sqrt(new Vector3(mat[2, 0], mat[2, 1], mat[2, 2]).LengthSquared()),
            };

            Matrix3x3 rotation = mat;
            rotation[0, 0] /= Scale.X;
            rotation[0, 1] /= Scale.X;
            rotation[0, 2] /= Scale.X;
            rotation[1, 0] /= Scale.Y;
            rotation[1, 1] /= Scale.Y;
            rotation[1, 2] /= Scale.Y;
            rotation[2, 0] /= Scale.Z;
            rotation[2, 1] /= Scale.Z;
            rotation[2, 2] /= Scale.Z;

            Quaternion quat = new Quaternion(rotation);
            Rotation = new Vector4()
            {
                X = float.IsNaN(quat.X) ? 0 : quat.X,
                Y = float.IsNaN(quat.Y) ? 0 : quat.Y,
                Z = float.IsNaN(quat.Z) ? 0 : quat.Z,
                W = float.IsNaN(quat.W) ? 0 : quat.W,
            };
        }
    }
}