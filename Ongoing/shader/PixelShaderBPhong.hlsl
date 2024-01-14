cbuffer FaceColors {
    float3 face_color[6];
};

cbuffer Light {
    float3 lightPos;
    float3 lightColor;
    float attConst;
    float attLin;
    float attQuad;
};

cbuffer Material {
    float3 diffuseColor;
    float3 specularColor;
    float specularShinyness;
    float3 ambientColor;
    float3 emmisiveColor;
};

float4 main( float3 worldPos : Position, float3 n : Normal ) : SV_Target {
    n = normalize(n);
    const float3 vToL = lightPos - worldPos;
    const float3 vToLDir = normalize(vToL);
    const float vToLDist = length(vToL);
    const float3 eyeDir = -normalize(worldPos);
    const float3 halfway = normalize(vToLDir + eyeDir);

    const float att = 1.0f / (attConst + attLin * vToLDist + attQuad * vToLDist * vToLDist);
    const float rambertCoef = max(dot(vToLDir, n), 0);
    const float3 diffuse = rambertCoef * diffuseColor;
    const float3 specular = pow( max( dot(halfway, n), 0 ), specularShinyness ) * specularColor;
    const float3 ambient = ambientColor;

    return float4( saturate(diffuse + specular + ambient + emmisiveColor), 1.0f );
}