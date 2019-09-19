#pragma once

class Patch;
class Program;

//-------------------------------------------------------------------------------------------------
// Patch animator base class

class Animator
{
public:
    Animator() {}
    virtual ~Animator() {}

    void setPatch(Patch* patch) {patch_ = patch;}
    virtual void update(double time) {}
    virtual void setAdditionalUniforms(Program& prg) {}
    virtual void draw() {}
    virtual void getProgramPaths(std::string& vertex, std::string& fragment);
    virtual void generateNormals();

protected:
    Patch* patch_;
};

//-------------------------------------------------------------------------------------------------
// Fully random heights every frame

class AnimatorRandom : public Animator
{
    virtual void update(double time);
};

//-------------------------------------------------------------------------------------------------
// Animates waves as single sine wave

class AnimatorSine : public Animator
{
    virtual void update(double time);
};
